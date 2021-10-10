#pragma once
#include<Siv3D.hpp>
#include"Toml_Load_Function.h"
// �V�[���̖��O
enum class State
{
	Title,
	Game,
	GameOver
};
// �Q�[���f�[�^
struct GameData
{
	//����̃X�R�A
	int32 now = 0;
	// �n�C�X�R�A
	int32 highScore = 0;
	int32 nowScore = 0;
	TOMLConfig config =  TOMLConfig(U"example/config/config.toml");
};

// �V�[���Ǘ��N���X
using MyApp = SceneManager<State, GameData>;
