#ifndef TIME_H
#define TIME_H

namespace Framework {
    class Time {
    public:
        Time();
        void Update();

        float deltaTime;

    private:
        float lastFrame;
    };
}

#endif // ! TIME_H
