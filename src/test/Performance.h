#pragma once

#include <algorithm> // std::max
#include <vector> // std::vector

#include "Timer.h"

void testVectorElemRead(std::vector<float> const& data);
void testVectorElemWrite(std::vector<float> const& data);
void testVectorAdd(std::vector<float> const& data);
void testVectorSub(std::vector<float> const& data);
void testScalarMul(std::vector<float> const& data);
void testScalarDiv(std::vector<float> const& data);
void testVectorLength(std::vector<float> const& data);
void testVectorLengthFast(std::vector<float> const& data);
void testVectorNormalize(std::vector<float> const& data);
void testVectorNormalizeFast(std::vector<float> const& data);
void testVectorDot(std::vector<float> const& data);
void testVectorCross(std::vector<float> const& data);
void testVectorProject(std::vector<float> const& data);
void testVectorReject(std::vector<float> const& data);
void testVectorReflect(std::vector<float> const& data);
void testMatrixScalar(std::vector<float> const& data);
void testMatrixVector(std::vector<float> const& data);
void testMatrixMatrix(std::vector<float> const& data);
void testHitSphere(std::vector<float> const& data);
void testHitCapsule(std::vector<float> const& data);
void testTraceScene(std::vector<float> const& data);
