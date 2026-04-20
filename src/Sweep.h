#ifndef SWEEP_H
#define SWEEP_H

#include <string>

class Sweep
{
public:
    Sweep() = default;

    const std::string& getName() const;
    const std::string& getWavPath() const;
    const std::string& getPosPath() const;

    void setName(const std::string& name);
    void setWavPath(const std::string& path);
    void setPosPath(const std::string& path);

private:
    std::string name;
    std::string wavPath;
    std::string posPath;
};

#endif // SWEEP_H
