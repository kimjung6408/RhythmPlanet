#pragma once

class ScoreSystem
{
private:
	unsigned long long score;
	float ScoreAdditionTimeDiff;
	int ScoreAccelCount;
	float ScoreTimeSum;
	float ScoreAccelerateTimeSum;
	float BPM;

public:
	ScoreSystem(float BPM)
		:BPM(BPM)
	{
		score = 0;
		ScoreAdditionTimeDiff = 30.0f / BPM;
		ScoreAccelCount = 0;
		ScoreTimeSum = 0;
		ScoreAccelerateTimeSum = 0;
	}

	//플레이어가 충돌했을 때, 점수의 가속을 초기화함.
	void InitializeAcceleration()
	{
		ScoreAccelerateTimeSum = 0;
		ScoreTimeSum = 0;
		ScoreAccelCount = 0;
		ScoreAdditionTimeDiff = 30.0f/BPM;
	}

	void Update(float dt)
	{
		ScoreTimeSum += dt;

		if (ScoreAccelCount < 5)
		{
			ScoreAccelerateTimeSum += dt;
		}

		//스코어 +할 수 있는 시간이 되면
		if (ScoreTimeSum >= ScoreAdditionTimeDiff)
		{
			ScoreTimeSum -= ScoreAdditionTimeDiff;

			//10점의 스코어를 더한다.
			score += 10;
		}

		//충돌 없이 7초가 경과하면
		if (ScoreAccelerateTimeSum >= 7.0f)
		{

			if (ScoreAccelCount < 5)
			{
				ScoreAccelCount += 1;

				//score 10점을 더하는 시간간격을 반으로 줄여서 가속한다.
				ScoreAdditionTimeDiff /= 2.0f;
			}

			ScoreAccelerateTimeSum -= 7.0f;
		}
	}

	unsigned long long GetScore()
	{
		return score;
	}
};