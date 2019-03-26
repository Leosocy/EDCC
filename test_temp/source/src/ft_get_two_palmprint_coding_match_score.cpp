#include <ft_get_two_palmprint_coding_match_score.h>

void ft_get_two_palmprint_coding_match_score::SetUp()
{
    ft_edcc_base::SetUp();

    bufMaxLen = 1024 * 16;
    this->firstCodingBuff = (unsigned char*)malloc(sizeof(unsigned char) * bufMaxLen);
    this->secondCodingBuff = (unsigned char*)malloc(sizeof(unsigned char) * bufMaxLen);
    memset(firstCodingBuff, 0, sizeof(unsigned char) * bufMaxLen);
    memset(secondCodingBuff, 0, sizeof(unsigned char) * bufMaxLen);

    this->matchScoreWithPalmprint = 0;
    this->matchScoreWithPalmprintCoding = 0;
    this->firstImagePath = NULL;
    this->secondImagePath = NULL;
    this->matchScoreWithPalmprintCodingVec.clear();
    this->matchScoreWithPalmprintVec.clear();

    SetConfigPath(CORRECT_CONFIG_PATH);
}
void ft_get_two_palmprint_coding_match_score::TearDown()
{
    ft_edcc_base::TearDown();

    FREE_CHAR_ARRAY(this->firstCodingBuff);
    FREE_CHAR_ARRAY(this->secondCodingBuff);
    FREE_CHAR_ARRAY(this->firstImagePath);
    FREE_CHAR_ARRAY(this->secondImagePath);
}

void ft_get_two_palmprint_coding_match_score::ExcuteInterface()
{
    interRet = GetTwoPalmprintCodingMatchScore(this->firstCodingBuff,
                                               this->secondCodingBuff,
                                               &this->matchScoreWithPalmprintCoding);

    int lRet = GetTwoPalmprintMatchScore(this->firstImagePath,
                                         this->secondImagePath,
                                         this->configPath,
                                         &this->matchScoreWithPalmprint);
    this->matchScoreWithPalmprintVec.push_back(this->matchScoreWithPalmprint);
    this->matchScoreWithPalmprintCodingVec.push_back(this->matchScoreWithPalmprintCoding);
    EXPECT_EQ(lRet, EDCC_SUCCESS);
}

void ft_get_two_palmprint_coding_match_score::GenFirstPalmprintCodingBuff(const char *firstPalmprintImagePath)
{
    GenCodingBuff(firstPalmprintImagePath, &this->firstImagePath, &this->firstCodingBuff);
}
void ft_get_two_palmprint_coding_match_score::GenSecondPalmprintCodingBuff(const char *secondPalmprintImagePath)
{
    GenCodingBuff(secondPalmprintImagePath, &this->secondImagePath, &this->secondCodingBuff);
}

void ft_get_two_palmprint_coding_match_score::GenCodingBuff(const char *imagePath, char **dstImagePath, unsigned char **codingBuff)
{
    EXPECT_TRUE(imagePath != NULL);
    FREE_CHAR_ARRAY(*dstImagePath);
    MALLOC_CHAR_ARRAY(*dstImagePath, imagePath, PATH_LEN);
    int ret = GetEDCCCoding(*dstImagePath,
                            this->configPath,
                            this->bufMaxLen,
                            *codingBuff,
                            &this->bufLen);
    EXPECT_EQ(ret, EDCC_SUCCESS);
    EXPECT_NE(bufLen, 0);
}

void ft_get_two_palmprint_coding_match_score::GenMatchScoreBaseline()
{
    SetCodingModeCompression();
    CheckCodingModeCompression();
    SetMatchingModeReliable();
    CheckMatchingModeReliable();
    GenFirstPalmprintCodingBuff(ID1_FIRST_PALMPRINT);
    GenSecondPalmprintCodingBuff(ID1_SECOND_PALMPRINT);
    ExcuteInterface();
    CheckInterfaceRet(EDCC_SUCCESS);
    CheckMatchScoreNE(0.0);
    CheckMatchScoreEQToPalmprint();
}

void ft_get_two_palmprint_coding_match_score::SetFirstPalmprintCoding(unsigned char *codingBuff)
{
    if(this->firstCodingBuff != NULL) {
        free(this->firstCodingBuff);
        this->firstCodingBuff = NULL;
    }
    this->firstCodingBuff = codingBuff;
}
void ft_get_two_palmprint_coding_match_score::SetSecondPalmprintCoding(unsigned char *codingBuff)
{
    if(this->secondCodingBuff != NULL) {
        free(this->secondCodingBuff);
        this->secondCodingBuff = NULL;
    }
    this->secondCodingBuff = codingBuff;
}

void ft_get_two_palmprint_coding_match_score::ChangePalmprintCoding()
{
    EXPECT_NE((size_t)secondCodingBuff, (size_t)NULL);
    if(secondCodingBuff == NULL) {
        return;
    }
    memset(secondCodingBuff + bufLen - 8, 0, 8);
}

void ft_get_two_palmprint_coding_match_score::CheckMatchScoreEQ(double expectScore)
{
    EXPECT_EQ(this->matchScoreWithPalmprintCoding, expectScore);
}
void ft_get_two_palmprint_coding_match_score::CheckMatchScoreNE(double expectScore)
{
    EXPECT_NE(this->matchScoreWithPalmprintCoding, expectScore);
}
void ft_get_two_palmprint_coding_match_score::CheckMatchScoreEQToPalmprint()
{
    EXPECT_EQ(this->matchScoreWithPalmprintCoding, this->matchScoreWithPalmprint);
}
void ft_get_two_palmprint_coding_match_score::CheckMatchScoreNEToPalmprint()
{
    EXPECT_NE(this->matchScoreWithPalmprintCoding, this->matchScoreWithPalmprint);
}

void ft_get_two_palmprint_coding_match_score::CheckMatchScoreEQWhenCodingModeAndMatchingModeChanged()
{
    EXPECT_EQ(this->matchScoreWithPalmprintVec.size(), 2);
    EXPECT_EQ(this->matchScoreWithPalmprintCodingVec.size(), 2);
    EXPECT_EQ(this->matchScoreWithPalmprintVec[0], this->matchScoreWithPalmprintVec[1]);
    EXPECT_EQ(this->matchScoreWithPalmprintCodingVec[0], this->matchScoreWithPalmprintCodingVec[1]);
    EXPECT_EQ(this->matchScoreWithPalmprintVec[0], this->matchScoreWithPalmprintCodingVec[0]);
}

TEST_F(ft_get_two_palmprint_coding_match_score, Given_NULL_PalmprintCodingBuff_When_Get_Score_Then_EDCC_NULL_POINTER_ERROR)
{
    GenFirstPalmprintCodingBuff(ID1_FIRST_PALMPRINT);
    SetFirstPalmprintCoding(NULL);
    GenSecondPalmprintCodingBuff(ID1_SECOND_PALMPRINT);

    ExcuteInterface();

    CheckInterfaceRet(EDCC_NULL_POINTER_ERROR);
}

TEST_F(ft_get_two_palmprint_coding_match_score, Given_Two_Diff_PalmprintCoding_When_Get_Score_Then_EDCC_SUCCESS_And_MatchScoreNE0)
{
    SetCodingModeCompression();
    CheckCodingModeCompression();
    SetMatchingModeSpeed();
    CheckMatchingModeSpeed();
    SetAllParamsCorrect();
    GenFirstPalmprintCodingBuff(ID1_FIRST_PALMPRINT);
    GenSecondPalmprintCodingBuff(ID1_SECOND_PALMPRINT);

    ExcuteInterface();

    CheckInterfaceRet(EDCC_SUCCESS);
    CheckMatchScoreNE(0.0);
    CheckMatchScoreEQToPalmprint();
}

TEST_F(ft_get_two_palmprint_coding_match_score, Given_Two_Same_PalmprintCoding_When_Get_Score_Then_EDCC_SUCCESS_And_MatchScoreEQ1)
{
    SetCodingModeCompression();
    CheckCodingModeCompression();
    SetMatchingModeReliable();
    CheckMatchingModeReliable();
    GenFirstPalmprintCodingBuff(ID1_FIRST_PALMPRINT);
    GenSecondPalmprintCodingBuff(ID1_FIRST_PALMPRINT);

    ExcuteInterface();

    CheckInterfaceRet(EDCC_SUCCESS);
    CheckMatchScoreEQ(1.0);
    CheckMatchScoreEQToPalmprint();
}

TEST_F(ft_get_two_palmprint_coding_match_score, Given_Two_PalmprintCoding_With_Diff_Config_When_Get_Score_Then_EDCC_CODINGS_DIFF_CONFIG)
{
    SetCodingModeCompression();
    CheckCodingModeCompression();
    SetMatchingModeReliable();
    CheckMatchingModeReliable();
    GenFirstPalmprintCodingBuff(ID1_FIRST_PALMPRINT);
    SetConfigPath(INCREMENTAL_CONFIG_PATH);
    GenSecondPalmprintCodingBuff(ID1_SECOND_PALMPRINT);

    ExcuteInterface();

    CheckInterfaceRet(EDCC_CODINGS_DIFF_CONFIG);
    CheckMatchScoreEQ(0.0);
    CheckMatchScoreNEToPalmprint();
}

TEST_F(ft_get_two_palmprint_coding_match_score, Given_PalmprintCoding_Invalid_Config_When_Get_Score_Then_EDCC_CODING_INVALID)
{
    SetCodingModeCompression();
    CheckCodingModeCompression();
    SetMatchingModeReliable();
    CheckMatchingModeReliable();
    GenFirstPalmprintCodingBuff(ID1_FIRST_PALMPRINT);
    GenSecondPalmprintCodingBuff(ID1_SECOND_PALMPRINT);
    ChangePalmprintCoding();

    ExcuteInterface();

    CheckInterfaceRet(EDCC_CODING_INVALID);
    CheckMatchScoreEQ(0.0);
    CheckMatchScoreNEToPalmprint();
}

TEST_F(ft_get_two_palmprint_coding_match_score, Given_CodingModeCompressionAndMatchingModeReliable_When_ChangeTo_CodingModeCompressionAndMatchingModeSpeed_Then_MatchScoreEQ)
{
    GenMatchScoreBaseline();

    SetCodingModeCompression();
    CheckCodingModeCompression();
    SetMatchingModeSpeed();
    CheckMatchingModeSpeed();
    GenFirstPalmprintCodingBuff(ID1_FIRST_PALMPRINT);
    GenSecondPalmprintCodingBuff(ID1_SECOND_PALMPRINT);
    ExcuteInterface();
    CheckInterfaceRet(EDCC_SUCCESS);
    CheckMatchScoreNE(0.0);
    CheckMatchScoreEQToPalmprint();

    CheckMatchScoreEQWhenCodingModeAndMatchingModeChanged();
}

TEST_F(ft_get_two_palmprint_coding_match_score, Given_CodingModeCompressionAndMatchingModeReliable_When_ChangeTo_CodingModeFastAndMatchingModeReliable_Then_MatchScoreEQ)
{
    GenMatchScoreBaseline();

    SetCodingModeFast();
    CheckCodingModeFast();
    SetMatchingModeReliable();
    CheckMatchingModeReliable();
    GenFirstPalmprintCodingBuff(ID1_FIRST_PALMPRINT);
    GenSecondPalmprintCodingBuff(ID1_SECOND_PALMPRINT);
    ExcuteInterface();
    CheckInterfaceRet(EDCC_SUCCESS);
    CheckMatchScoreNE(0.0);
    CheckMatchScoreEQToPalmprint();

    CheckMatchScoreEQWhenCodingModeAndMatchingModeChanged();
}

TEST_F(ft_get_two_palmprint_coding_match_score, Given_CodingModeCompressionAndMatchingModeReliable_When_ChangeTo_CodingModeFastAndMatchingModeSpeed_Then_MatchScoreEQ)
{
    GenMatchScoreBaseline();

    SetCodingModeFast();
    CheckCodingModeFast();
    SetMatchingModeSpeed();
    CheckMatchingModeSpeed();
    GenFirstPalmprintCodingBuff(ID1_FIRST_PALMPRINT);
    GenSecondPalmprintCodingBuff(ID1_SECOND_PALMPRINT);
    ExcuteInterface();
    CheckInterfaceRet(EDCC_SUCCESS);
    CheckMatchScoreNE(0.0);
    CheckMatchScoreEQToPalmprint();

    CheckMatchScoreEQWhenCodingModeAndMatchingModeChanged();
}