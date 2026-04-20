#include "Sweep.h"

const std::string& Sweep::getName() const
{
    return name;
}

const std::string& Sweep::getWavPath() const
{
    return wavPath;
}

const std::string& Sweep::getPosPath() const
{
    return posPath;
}

void Sweep::setName(const std::string& newName)
{
    name = newName;
}

void Sweep::setWavPath(const std::string& path)
{
    wavPath = path;
}

void Sweep::setPosPath(const std::string& path)
{
    posPath = path;
}
