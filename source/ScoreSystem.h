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

	//�÷��̾ �浹���� ��, ������ ������ �ʱ�ȭ��.
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

		//���ھ� +�� �� �ִ� �ð��� �Ǹ�
		if (ScoreTimeSum >= ScoreAdditionTimeDiff)
		{
			ScoreTimeSum -= ScoreAdditionTimeDiff;

			//10���� ���ھ ���Ѵ�.
			score += 10;
		}

		//�浹 ���� 7�ʰ� ����ϸ�
		if (ScoreAccelerateTimeSum >= 7.0f)
		{

			if (ScoreAccelCount < 5)
			{
				ScoreAccelCount += 1;

				//score 10���� ���ϴ� �ð������� ������ �ٿ��� �����Ѵ�.
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