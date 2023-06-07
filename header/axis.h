#ifndef AXIS_H
#define AXIS_H

#pragma once

#include <glm/glm.hpp>
#include <vector>
using std::vector;
using glm::vec3;

enum BasisVectorCartesian : unsigned char
{
    i_hat       = 0,
    j_hat       = 1 << 0,
    k_hat       = 1 << 1,
};
enum BasisVectorPolar : unsigned char
{
    r_hat       = 1 << 2,
    theta_hat   = 1 << 3
};

struct Cube;
struct GameText;
struct GlobalTexture;
struct Window;

struct Axis
{
    Axis( unsigned char CartnBasisVector, vec3 Origin, GlobalTexture *tex, int Length, int Step, Window *container );
    Axis( unsigned char PolarBasisVector, vec3 Origin, GlobalTexture *tex, int Length, int Step, int OtherValue, Window *container );

    unsigned char dir;
    unsigned long long Length;
    unsigned long long Step;

    vector<Cube *> boxes;
    GameText *AxisLabel;
    vector<GameText *> ValueLabels;
};

#endif
