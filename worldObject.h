#pragma once
#include<Siv3D.hpp>
#include"Enemy.h"
#include"Toml_Load_Function.h"


Array<Vec2> CreateRightFrame(const Vec2& RightAnchor)
{
	Array<Vec2> points = { Vec2(7, -1.5),RightAnchor };

	return points;
}

Array<Vec2> CreateLeftFrame(const Vec2& leftAnchor)
{
	Array<Vec2> points = { leftAnchor, Vec2(-7, -1.5) };
	return points;
}
ColorF GetColor(const P2Body& body, const Array<P2BodyID>& list)
{
	return list.includes(body.id()) ? Palette::White : Palette::Orange;
}

String GetEnemyTexturename(size_t i)
{
	const Array<String> names = { U"crab", U"octopus" };
	return names[i];
}
//Enemy�Ɋւ���f�[�^��struct�ɂ܂Ƃ߂�
struct EnemyManager
{
	Array<P2Body> P2_enemies;
	Array<P2BodyID> enemies_ids;
	Array<Enemy> enemies;
	Array<EnemyData> enemy_data;
};

//bumper�Ɋւ���f�[�^��struct�ɂ܂Ƃ߂�
struct bumperManager {
	//�o���p�[
	Array<P2Body> bumpers;
	P2Body Itembumpers;
	Array<P2Body> Round_bumpers;
	Array<P2BodyID> bumper_id;
};

//flipper�Ɋւ���f�[�^��struct�ɂ܂Ƃ߂�
struct FlipperManager
{
	P2Body leftFlipper, rightFlipper;
	Vec2 leftFlipperAnchor = Vec2(-2.72, 1), rightFlipperAnchor = Vec2(2.72, 1);
	P2PivotJoint leftJoint, rightJoint;
};


/// @brief �������Z�Ɋւ���S�Ă̏����Ǘ�
struct WorldObjects
{
	P2World m_world = P2World(9.0);

	EnemyManager enemy_data;
	Array<RectF> Itemrects;
	bumperManager bumper_data;

	//�A�C�e���̕ϐ�
	bool getItem = false;

	// �Œ�̘g
	Array<P2Body> frames;

	////�t���b�p�[
	FlipperManager flipper;

	////����

	void clear()
	{
		flipper.leftJoint = P2PivotJoint{};
		flipper.rightJoint = P2PivotJoint{};
		flipper.leftFlipper = P2Body{};
		flipper.rightFlipper = P2Body{};
	}

};

/// @brief �R���t�B�O�����Ƃ� WorldObjects ���X�V����
/// @param config �R���t�B�O
/// @param worldObjects WorldObjects
void UpdateWorldObjects(const TOMLConfig& config, WorldObjects& worldObjects)
{
	Array<Array<Vec2>> frames = config.LoadStraightFramePos();
	Array<Vec2> RoundBumper = config.LoadRoundbumperData();
	Array<Triangle> triangle = config.LoadTriangleData();
	Array<Point> range = config.LoadgetRange();
	Array<EnemyData> enemy = config.LoadEnemyData();
	//�t���[���̃����[�h
	for (size_t i = 0; i < frames.size(); i++)
	{
		worldObjects.frames << worldObjects.m_world.createStaticLineString(Vec2(0, 0),
			LineString({ frames[i] }));
	}
	for (size_t i = 0; i < RoundBumper.size(); i++)
	{
		worldObjects.bumper_data.bumpers << worldObjects.m_world.createStaticCircle(Vec2(RoundBumper[i]), 0.9, P2Material(0.65, 0.65));
	}

	worldObjects.frames << worldObjects.m_world.createStaticLineString(Vec2(0, 0), LineString(CreateLeftFrame(worldObjects.flipper.leftFlipperAnchor)));
	worldObjects.frames << worldObjects.m_world.createStaticLineString(Vec2(0, 0), LineString(CreateRightFrame(worldObjects.flipper.rightFlipperAnchor)));

	worldObjects.flipper.leftFlipper = worldObjects.m_world.createDynamicRect(worldObjects.flipper.leftFlipperAnchor, RectF(0.0, 0.04, 2.15, 0.45), P2Material(0.25, 0.0));
	worldObjects.flipper.leftJoint = worldObjects.m_world.createPivotJoint(worldObjects.frames[0], worldObjects.flipper.leftFlipper, worldObjects.flipper.leftFlipperAnchor).setLimits(-20_deg, 25_deg).setLimitEnabled(true);
	worldObjects.flipper.rightFlipper = worldObjects.m_world.createDynamicRect(worldObjects.flipper.rightFlipperAnchor, RectF(-2.15, 0.04, 2.15, 0.45), P2Material(0.25, 0.0));
	worldObjects.flipper.rightJoint = worldObjects.m_world.createPivotJoint(worldObjects.frames[1], worldObjects.flipper.rightFlipper, worldObjects.flipper.rightFlipperAnchor).setLimits(-25_deg, 20_deg).setLimitEnabled(true);
	for (size_t i = 0; i < range.size(); i++)
	{
		worldObjects.frames << worldObjects.m_world.createStaticLineString(Vec2(0, 0),
			LineString(Range(range[i].x, range[i].y).map([=](int32 j)
				{ return  OffsetCircular(config.LoadRoundFrameData(j)[i]).toVec2(); })));
	}
	//�o���p�[�̃����[�h
	for (size_t i = 0; i < triangle.size(); i++)
	{
		worldObjects.bumper_data.bumpers << worldObjects.m_world.createStaticTriangle(Vec2(0, 0), Triangle(triangle[i]), P2Material(1.0, 0.8));
	}

	if (worldObjects.bumper_data.Itembumpers.isEmpty() && worldObjects.getItem)
	{
		worldObjects.bumper_data.Itembumpers = worldObjects.m_world.createStaticRect(Vec2(config.loadStraightBumper().pos), RectF(0, 0, { config.loadStraightBumper().size }), P2Material(2.65, 2.65));
	}
	for (size_t i = 0; i < config.LoadRoundbumperData().size(); i++)
	{
		worldObjects.bumper_data.bumpers << worldObjects.m_world.createStaticCircle(Vec2(config.LoadRoundbumperData()[i]), 0.9, P2Material(0.65, 0.65));
	}
	//�G�̃����[�h
	for (size_t i = 0; i < enemy.size(); i++)
	{
		worldObjects.enemy_data.P2_enemies << worldObjects.m_world.createStaticQuad(Vec2(0, 0), Quad((RectF(enemy[i].rect.x,
			enemy[i].rect.y, enemy[i].rect.w + enemy[i].error.x,
			enemy[i].rect.h + enemy[i].error.y)).rotated(enemy[i].angle * 1_deg)), P2Material(0.6, 0.6));
		worldObjects.enemy_data.enemies_ids << worldObjects.enemy_data.P2_enemies[i].id();
	}
	for (size_t i = 0; i < enemy.size(); i++)
	{
		worldObjects.enemy_data.enemies << Enemy(GetEnemyTexturename(i), { enemy[i].rect.pos }, { enemy[i].rect.w,enemy[i].rect.h },
			enemy[i].error, enemy[i].angle, worldObjects.m_world, worldObjects.enemy_data.P2_enemies[i], false);
		worldObjects.enemy_data.P2_enemies << worldObjects.m_world.createStaticQuad(Vec2(0, 0), Quad((RectF(worldObjects.enemy_data.enemies[i].getPos().x, worldObjects.enemy_data.enemies[i].getPos().y,
			worldObjects.enemy_data.enemies[i].getSize().x + enemy[i].error.x, worldObjects.enemy_data.enemies[i].getSize().y + enemy[i].error.y)).rotated(worldObjects.enemy_data.enemies[i].getAngle() * 1_deg)), P2Material(0.6, 0.6));
	}

	for (size_t i = 0; i < config.LoadItemRect().size(); i++)
	{
		worldObjects.Itemrects << config.LoadItemRect()[i];
	}


}
