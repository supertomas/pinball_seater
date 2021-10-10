#pragma once
# include <Siv3D.hpp> // OpenSiv3D v0.4.2

/* ������ BubbleTexture �N���X */

class BubbleTexture
{
private:

    // �A��`�悷������e�N�X�`��
    MSRenderTexture m_texture;

    // �A��\������ Circle �̔z��
    Array<Circle> m_bubbles;

    // �A�̔������[�g�̍ő�l�i��/�b�j
    static constexpr double MaxSpawnRate = 300.0;

    // �A�̃X�s�[�h�̍ŏ��l�i�s�N�Z��/�b�j
    static constexpr double MinSpeed = 10.0;

    // �A�̔������[�g�i��/�b�j
    double m_spawnRate = 10.0;

    // �A�̃X�s�[�h�i�s�N�Z��/�b�j
    double m_speed = 40.0;

    // �A�̍ŏ��T�C�Y
    double m_minSize = 0.0;

    // �A�̍ő�T�C�Y
    double m_maxSize = 0.0;

    // �A�����X�V�p�̃J�E���^
    double m_timeAccum = 0.0;

public:

    BubbleTexture() = default;

    // �R���X�g���N�^
    BubbleTexture(const Size& size, double spawnRate, double speed, double minSize, double maxSize)
        : m_texture(size)
        , m_spawnRate(Clamp(spawnRate, 0.0, MaxSpawnRate))
        , m_speed(Max(speed, MinSpeed))
        , m_minSize(minSize)
        , m_maxSize(maxSize)
    {
        // prewarm
        const double prewarmTime = (size.y + m_maxSize * 2) / speed;
        const double prewarmStepTime = 1.0 / 60.0;

        for (int32 i = 0; i < (prewarmTime / prewarmStepTime); ++i)
        {
            update(prewarmStepTime);
        }
    }

    // �A�̍X�V
    void update(double deltaTime)
    {
        // �A�̈ړ�
        {
            const double speedBase = ((m_minSize + m_maxSize) * 0.5);

            for (auto& bubble : m_bubbles)
            {
                bubble.y -= deltaTime * m_speed * (bubble.r / speedBase);
            }
            m_bubbles.remove_if([maxSize = m_maxSize](const Circle& c) { return c.y < -maxSize; });
        }

        // �A�̔���
        {
            m_timeAccum += deltaTime;

            const double spawnTime = 1.0 / m_spawnRate;

            while (m_timeAccum > spawnTime)
            {
                const RectF spawnArea(-m_maxSize, m_texture.height() + m_maxSize, m_texture.width() + (m_maxSize * 2), m_maxSize);

                m_bubbles.emplace_back(RandomVec2(spawnArea), Random(m_minSize, m_maxSize));

                m_timeAccum -= spawnTime;
            }
        }
    }

    // �A������e�N�X�`���ɕ`��
    void render(const ColorF& backgroundColor)
    {
        m_texture.clear(backgroundColor);
        {
            ScopedRenderTarget2D rt(m_texture);
            ScopedRenderStates2D blend(BlendState::Additive);

            for (const auto& bubble : m_bubbles)
            {
                const double a = EaseInQuad(0.15 + bubble.y / m_texture.height() * 0.45);
                bubble.draw(ColorF(1.0, a));
            }
        }
        Graphics2D::Flush();
        m_texture.resolve();
    }

    // �����e�N�X�`�����擾
    const Texture& getTexture() const
    {
        return m_texture;
    }
};
