/*************************************************************************
    > File Name: IO..cc
    > Author: Leosocy
    > Mail: 513887568@qq.com 
    > Created Time: 2017/07/26 21:29:10
 ************************************************************************/
#include <IO.h>
#include <iostream>
#include <assert.h>

#define PALMPRINT_GROUP_FORMAT  "{\n\
\"identity\" : [\n\
    \"path1\"\n\
    \"path2\"\n\
    ]\n\
}"
EDCC::IO::IO()
{
    paramsSet.insert("imageSize");
    paramsSet.insert("laplaceKernelSize");
    paramsSet.insert("gaborKernelSize");
    paramsSet.insert("gaborDirections");
}

int EDCC::IO::loadConfig(_IN ifstream &in)
{
    Json::Value root;
    Json::Reader reader;
    Json::Value::Members members;
    if(!reader.parse(in, root)) {
        EDCC_Log("Parse config.json failed, please confirm the file is exists.\n");
        return EDCC_LOAD_CONFIG_FAIL;
    }
    members = root.getMemberNames();
    for(Json::Value::Members::iterator it = members.begin(); 
            it != members.end(); ++it) {
        if(!root[*it].isObject() || root[*it]["default"].isNull()) {
            EDCC_Log("Parse config.json failed, you can only change the value of \
                     \"default\" label in this file.\n");
            return EDCC_LOAD_CONFIG_FAIL;
        } else {
            if(paramsSet.find(*it) != paramsSet.end()) {
                if(configMap.find(*it) != configMap.end()) {
                    configMap.at(*it) = root[*it]["default"].asInt();
                } else {
                    configMap.insert(map<string, int>::value_type(*it, root[*it]["default"].asInt()));
                }         
            } else {
                EDCC_Log("Illegal configuration parameters.\n");
                return EDCC_LOAD_CONFIG_FAIL;
            }
        }
    }

    return EDCC_SUCCESS;
}

int EDCC::IO::loadPalmprintGroup(_IN ifstream &in, _Inout vector<PalmprintCode> &groupVec)
{
    Json::Value root;
    Json::Reader reader;
    Json::Value::Members members;

    if(!reader.parse(in, root)) {
        EDCC_Log("Parse json failed. Don't change the json format. You need to confirm the format like this.\n");
        EDCC_Log(PALMPRINT_GROUP_FORMAT"\n");
        return EDCC_LOAD_TAINING_SET_FAIL;
    }
    members = root.getMemberNames();
    for(Json::Value::Members::iterator it = members.begin(); 
            it != members.end(); ++it) {
        if(!root[*it].isArray()) {
            EDCC_Log("Don't change the json format. You need to confirm the format like this.\n");
            EDCC_Log(PALMPRINT_GROUP_FORMAT"\n");
            return EDCC_LOAD_TAINING_SET_FAIL;
        }
        Json::Value imageList = root[*it];
        for(size_t imageIndex = 0; imageIndex < imageList.size(); ++imageIndex) {
            if(imageList[(unsigned)imageIndex].isString()) {
                PalmprintCode newOne((*it).c_str(), imageList[(unsigned)imageIndex].asString().c_str());
                groupVec.push_back(newOne);
            }
        }
    }

    return EDCC_SUCCESS;
}

int EDCC::IO::loadPalmprintFeatureData(_IN ifstream &in, _Inout vector<PalmprintCode> &data)
{
    Json::Value root;
    Json::Reader reader;
    Json::Value::Members members;
    if(!reader.parse(in, root)) {
        EDCC_Log("Load Palmprint Features Data failed. Don't change the json format.\n");
        return EDCC_LOAD_FEATURES_FAIL;
    }
    for(set<string>::iterator it = paramsSet.begin(); it != paramsSet.end(); ++it) {
        if(root[*it].isNull() || !root[*it].isInt()) {
            EDCC_Log("Load Palmprint Features Data failed. Make sure json file has config.\n");
            return EDCC_LOAD_CONFIG_FAIL;
        }
        if(configMap.find(*it) != configMap.end()) {
            configMap.at(*it) = root[*it].asInt();
        } else {
            configMap.insert(map<string, int>::value_type(*it, root[*it].asInt()));
        } 
    }
    members = root.getMemberNames();
    for(Json::Value::Members::iterator it = members.begin(); it != members.end(); ++it) {
        if(paramsSet.find(*it) == paramsSet.end()
           && root[*it].isObject()) {
            loadOneIdentityAllPalmprintFeatureData(*it, root[*it], data);
        }
    }

    return EDCC_SUCCESS;
}

int EDCC::IO::savePalmprintFeatureData(_IN ofstream &out, _IN vector<PalmprintCode> &data)
{
    if(!out.is_open()) {
        EDCC_Log("Output stream can't open.\n");
        return EDCC_SAVE_FEATURES_FAIL;
    }
    Json::Value root;
    for(set<string>::iterator it = paramsSet.begin(); it != paramsSet.end(); ++it) {
        if(configMap.find(*it) == configMap.end()) {
            EDCC_Log("If you want to train/predict, load config.json first.\
                    Or if you want incremental training/prediction, load trainData.json first.\n");
            return EDCC_SAVE_FEATURES_FAIL;
        } else {
            root[*it] = configMap.at(*it);
        }
    }

    for(vector<PalmprintCode>::iterator it = data.begin(); it != data.end(); ++it) {
        insert2JsonValue(*it, root);
    }
    out << root.toStyledString();

    return EDCC_SUCCESS;
}

void EDCC::IO::loadOneIdentityAllPalmprintFeatureData(_IN const string &identity,
                                                      _IN const Json::Value &value,
                                                      _Inout vector<PalmprintCode> &data)
{
    Json::Value::Members imagePathMembers;
    Json::Value::Members::iterator it;
    imagePathMembers = value.getMemberNames();

    for(it = imagePathMembers.begin(); it != imagePathMembers.end(); ++it) {
        PalmprintCode instanceCode(identity.c_str(), (*it).c_str());
        if(genEDCCoding(value[*it], instanceCode)) {
            data.push_back(instanceCode);
        }
    }
}

bool EDCC::IO::genEDCCoding(_IN const Json::Value &value, _Inout PalmprintCode &coding)
{
    if(value.isNull() || !value.isObject()) {
        EDCC_Log("Load Palmprint Features Data failed. Don't change the json format.\n");
        return false;
    }

    int imageSize = configMap.at("imageSize");
    Mat C(imageSize, imageSize, CV_8S);
    Mat Cs(imageSize, imageSize, CV_8S);
    
    if(value["C"].isNull() || !value["C"].isString()
       || value["Cs"].isNull() || !value["Cs"].isString()) {
        EDCC_Log("Load Palmprint Features Data failed. Don't change the json format.\n");
        return false;
    }
    coding.zipCodingC = EDCC::toUpper(value["C"].asString().c_str());
    coding.zipCodingCs = EDCC::toUpper(value["Cs"].asString().c_str());

    return true;
}

bool EDCC::IO::insert2JsonValue(_IN PalmprintCode &code, _Inout Json::Value &value)
{
    string identity = code.identity;
    string imagePath = code.imagePath;
    if(!value[identity].isNull() && !value[identity][imagePath].isNull()) {
        EDCC_Log("identity: %s\timagepath: %s\tis already exists.\n");
        return false;
    }
    Json::Value codeValue;
    setEDCCoding(code, codeValue);
    value[identity][imagePath] = codeValue;

    return true;
}

void EDCC::IO::setEDCCoding(_IN PalmprintCode &coding, _Inout Json::Value &value)
{
    Json::Value cValue, csValue;
    value["C"] = coding.zipCodingC;
    value["Cs"] = coding.zipCodingCs;
}

