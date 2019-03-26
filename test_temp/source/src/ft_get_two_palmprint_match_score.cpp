#include <ft_get_two_palmprint_match_score.h>

void ft_get_two_palmprint_match_score::SetUp()
{
    ft_edcc_base::SetUp();

    this->firstImagePath = NULL;
    this->secondImagePath = NULL;
    SetConfigPath(CORRECT_CONFIG_PATH);
    SetFirstPalmprintImagePath(ID1_FIRST_PALMPRINT);
    SetSecondPalmprintImagePath(ID1_SECOND_PALMPRINT);
}

void ft_get_two_palmprint_match_score::TearDown()
{
    ft_edcc_base::TearDown();

    FREE_CHAR_ARRAY(this->firstImagePath);
    FREE_CHAR_ARRAY(this->secondImagePath);
}

void ft_get_two_palmprint_match_score::SetFirstPalmprintImagePath(const char *path)
{
    FREE_CHAR_ARRAY(this->firstImagePath);
    MALLOC_CHAR_ARRAY(this->firstImagePath, path, PATH_LEN);
}

void ft_get_two_palmprint_match_score::SetSecondPalmprintImagePath(const char *path)
{
    FREE_CHAR_ARRAY(this->secondImagePath);
    MALLOC_CHAR_ARRAY(this->secondImagePath, path, PATH_LEN);
}

void ft_get_two_palmprint_match_score::CheckMatchScoreEQ(double expectScore)
{
    EXPECT_EQ(this->matchScore, expectScore);
}
void ft_get_two_palmprint_match_score::CheckMatchScoreNE(double expectScore)
{
    EXPECT_NE(this->matchScore, expectScore);
}

void ft_get_two_palmprint_match_score::ExcuteInterface()
{
    interRet = GetTwoPalmprintMatchScore(this->firstImagePath,
                                         this->secondImagePath,
                                         this->configPath,
                                         &this->matchScore);
}

TEST_F(ft_get_two_palmprint_match_score, Given_Some_Param_NULL_When_Get_Score_Then_EDCC_NULL_POINTER_ERROR)
{
    SetFirstPalmprintImagePath(NULL);
    
    ExcuteInterface();

    CheckInterfaceRet(EDCC_NULL_POINTER_ERROR);
}

TEST_F(ft_get_two_palmprint_match_score, Given_ConfigNotExists_When_Get_Score_Then_EDCC_LOAD_CONFIG_FAIL)
{
    SetConfigPath(NOT_EXISTS_CONFIG_PATH);

    ExcuteInterface();

    CheckInterfaceRet(EDCC_LOAD_CONFIG_FAIL);
}

TEST_F(ft_get_two_palmprint_match_score, Given_ConfigParamsError_When_Get_Score_Then_EDCC_LOAD_CONFIG_FAIL)
{
    ModifyConfigParams("gaborDirections", 32);
    CheckConfigParams("gaborDirections", 32);
    
    ExcuteInterface();

    CheckInterfaceRet(EDCC_LOAD_CONFIG_FAIL);
}

TEST_F(ft_get_two_palmprint_match_score, Given_ConfigParamsMiss_When_Get_Score_Then_EDCC_LOAD_CONFIG_FAIL)
{
    RemoveConfigParam("gaborDirections");
    CheckConfigParamNotExists("gaborDirections");
    
    ExcuteInterface();

    CheckInterfaceRet(EDCC_LOAD_CONFIG_FAIL);
}

TEST_F(ft_get_two_palmprint_match_score, Given_ConfigParamsOver_When_Get_Score_Then_EDCC_LOAD_CONFIG_FAIL)
{
    AppendConfigParam("overParameter", 66);
    CheckConfigParams("overParameter", 66);
    
    ExcuteInterface();

    CheckInterfaceRet(EDCC_LOAD_CONFIG_FAIL);
}

TEST_F(ft_get_two_palmprint_match_score, Given_One_Palmprint_Not_Exists_When_Get_Score_Then_EDCC_PALMPRINT_IMAGE_NOT_EXISTS)
{
    SetSecondPalmprintImagePath(NOT_EXISTS_PALMPRINT);
    
    ExcuteInterface();

    CheckInterfaceRet(EDCC_LOAD_PALMPRINT_IMAGE_FAIL);
}

TEST_F(ft_get_two_palmprint_match_score, Given_Two_Same_Image_When_Get_Score_Then_EDCC_SUCCESS_And_MatchScore_EQ1)
{
    SetFirstPalmprintImagePath(ID1_FIRST_PALMPRINT);
    SetSecondPalmprintImagePath(ID1_FIRST_PALMPRINT);
    
    ExcuteInterface();

    CheckInterfaceRet(EDCC_SUCCESS);
    CheckMatchScoreEQ(1.0);
}

TEST_F(ft_get_two_palmprint_match_score, Given_Two_Diff_Image_When_Get_Score_Then_EDCC_SUCCESS_And_MatchScore_NE1)
{
    SetFirstPalmprintImagePath(ID1_FIRST_PALMPRINT);
    SetSecondPalmprintImagePath(ID2_SECOND_PALMPRINT);
    
    ExcuteInterface();

    CheckInterfaceRet(EDCC_SUCCESS);
    CheckMatchScoreNE(1.0);
    CheckMatchScoreNE(0.0);
}
