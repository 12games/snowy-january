#ifndef GAME_H
#define GAME_H

class Game
{
public:
    virtual ~Game() {}

    virtual bool Setup() = 0;
    virtual void Resize(int width, int height) = 0;
    virtual void Update(int dt) = 0;
    virtual void Render() = 0;
    virtual void RenderUi() = 0;
    virtual void Destroy() = 0;

    static Game &Instantiate();

    int _width, _height;
};

#endif // GAME_H
