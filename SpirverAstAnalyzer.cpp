#include <SpirverAstAnalyzer.h>
#include <ostream>

using namespace Spirver;
using namespace Spirver::detail;

ShaderStat::ShaderStat()
{
	stats = new unsigned int[shaderStatTypesCount] { 0 };
	opCounts = new unsigned int[791]{ 0 };
}

ShaderStat::ShaderStat(const ShaderStat& o) : ShaderStat()
{
	std::copy(o.stats, o.stats + shaderStatTypesCount, stats);
	std::copy(o.opCounts, o.opCounts + 791, opCounts);
}

ShaderStat::ShaderStat(ShaderStat&& o)
{
	stats = o.stats;
	opCounts = o.opCounts;

	o.stats = nullptr;
	o.opCounts = nullptr;
}

ShaderStat& ShaderStat::operator=(const ShaderStat& o)
{
	std::copy(o.stats, o.stats + shaderStatTypesCount, stats);
	std::copy(o.opCounts, o.opCounts + 791, opCounts);

	return *this;
}

ShaderStat& ShaderStat::operator=(ShaderStat&& o)
{
	stats = o.stats;
	opCounts = o.opCounts;

	o.stats = nullptr;
	o.opCounts = nullptr;

	return *this;
}

bool ShaderStat::operator==(ShaderStat& o)
{
	return std::equal(stats, stats + shaderStatTypesCount, o.stats) &&
		std::equal(opCounts, opCounts + 791, o.opCounts);
}

bool ShaderStat::operator!=(ShaderStat& o)
{
	return !(*this == o);
}

std::ostream& operator<<(std::ostream& os, ShaderStat& s)
{
	for (int i = 0; i < shaderStatTypesCount; i++)
		os << shaderStatNames[i] << ": " << s.stats[i] << std::endl;
	return os;
}

const char* Spirver::shaderStatNames[14] = { "glslLines", "spirvLines", "math", "mathExpensive", "conditional",
			"assign", "access", "compare",
			"functionDef", "functionCall",
			"tempVar", "loop", "flow", "switches" };

void SpirverAstAnalyzerTraverser::AddToOpCounts(glslang::TOperator op)
{
	stat.opCounts[op]++;
}

const char* Spirver::glslangOperatorNames[] = { "EOpNull", "EOpSequence", "EOpLinkerObjects", "EOpFunctionCall", "EOpFunction", "EOpParameters", "EOpNegative", "EOpLogicalNot", "EOpVectorLogicalNot", "EOpBitwiseNot", "EOpPostIncrement", "EOpPostDecrement", "EOpPreIncrement", "EOpPreDecrement", "EOpCopyObject", "EOpConvInt8ToBool", "EOpConvUint8ToBool", "EOpConvInt16ToBool", "EOpConvUint16ToBool", "EOpConvIntToBool", "EOpConvUintToBool", "EOpConvInt64ToBool", "EOpConvUint64ToBool", "EOpConvFloat16ToBool", "EOpConvFloatToBool", "EOpConvDoubleToBool", "EOpConvBoolToInt8", "EOpConvBoolToUint8", "EOpConvBoolToInt16", "EOpConvBoolToUint16", "EOpConvBoolToInt", "EOpConvBoolToUint", "EOpConvBoolToInt64", "EOpConvBoolToUint64", "EOpConvBoolToFloat16", "EOpConvBoolToFloat", "EOpConvBoolToDouble", "EOpConvInt8ToInt16", "EOpConvInt8ToInt", "EOpConvInt8ToInt64", "EOpConvInt8ToUint8", "EOpConvInt8ToUint16", "EOpConvInt8ToUint", "EOpConvInt8ToUint64", "EOpConvUint8ToInt8", "EOpConvUint8ToInt16", "EOpConvUint8ToInt", "EOpConvUint8ToInt64", "EOpConvUint8ToUint16", "EOpConvUint8ToUint", "EOpConvUint8ToUint64", "EOpConvInt8ToFloat16", "EOpConvInt8ToFloat", "EOpConvInt8ToDouble", "EOpConvUint8ToFloat16", "EOpConvUint8ToFloat", "EOpConvUint8ToDouble", "EOpConvInt16ToInt8", "EOpConvInt16ToInt", "EOpConvInt16ToInt64", "EOpConvInt16ToUint8", "EOpConvInt16ToUint16", "EOpConvInt16ToUint", "EOpConvInt16ToUint64", "EOpConvUint16ToInt8", "EOpConvUint16ToInt16", "EOpConvUint16ToInt", "EOpConvUint16ToInt64", "EOpConvUint16ToUint8", "EOpConvUint16ToUint", "EOpConvUint16ToUint64", "EOpConvInt16ToFloat16", "EOpConvInt16ToFloat", "EOpConvInt16ToDouble", "EOpConvUint16ToFloat16", "EOpConvUint16ToFloat", "EOpConvUint16ToDouble", "EOpConvIntToInt8", "EOpConvIntToInt16", "EOpConvIntToInt64", "EOpConvIntToUint8", "EOpConvIntToUint16", "EOpConvIntToUint", "EOpConvIntToUint64", "EOpConvUintToInt8", "EOpConvUintToInt16", "EOpConvUintToInt", "EOpConvUintToInt64", "EOpConvUintToUint8", "EOpConvUintToUint16", "EOpConvUintToUint64", "EOpConvIntToFloat16", "EOpConvIntToFloat", "EOpConvIntToDouble", "EOpConvUintToFloat16", "EOpConvUintToFloat", "EOpConvUintToDouble", "EOpConvInt64ToInt8", "EOpConvInt64ToInt16", "EOpConvInt64ToInt", "EOpConvInt64ToUint8", "EOpConvInt64ToUint16", "EOpConvInt64ToUint", "EOpConvInt64ToUint64", "EOpConvUint64ToInt8", "EOpConvUint64ToInt16", "EOpConvUint64ToInt", "EOpConvUint64ToInt64", "EOpConvUint64ToUint8", "EOpConvUint64ToUint16", "EOpConvUint64ToUint", "EOpConvInt64ToFloat16", "EOpConvInt64ToFloat", "EOpConvInt64ToDouble", "EOpConvUint64ToFloat16", "EOpConvUint64ToFloat", "EOpConvUint64ToDouble", "EOpConvFloat16ToInt8", "EOpConvFloat16ToInt16", "EOpConvFloat16ToInt", "EOpConvFloat16ToInt64", "EOpConvFloat16ToUint8", "EOpConvFloat16ToUint16", "EOpConvFloat16ToUint", "EOpConvFloat16ToUint64", "EOpConvFloat16ToFloat", "EOpConvFloat16ToDouble", "EOpConvFloatToInt8", "EOpConvFloatToInt16", "EOpConvFloatToInt", "EOpConvFloatToInt64", "EOpConvFloatToUint8", "EOpConvFloatToUint16", "EOpConvFloatToUint", "EOpConvFloatToUint64", "EOpConvFloatToFloat16", "EOpConvFloatToDouble", "EOpConvDoubleToInt8", "EOpConvDoubleToInt16", "EOpConvDoubleToInt", "EOpConvDoubleToInt64", "EOpConvDoubleToUint8", "EOpConvDoubleToUint16", "EOpConvDoubleToUint", "EOpConvDoubleToUint64", "EOpConvDoubleToFloat16", "EOpConvDoubleToFloat", "EOpConvUint64ToPtr", "EOpConvPtrToUint64", "EOpConvUvec2ToPtr", "EOpConvPtrToUvec2", "EOpConvUint64ToAccStruct", "EOpConvUvec2ToAccStruct", "EOpAdd", "EOpSub", "EOpMul", "EOpDiv", "EOpMod", "EOpRightShift", "EOpLeftShift", "EOpAnd", "EOpInclusiveOr", "EOpExclusiveOr", "EOpEqual", "EOpNotEqual", "EOpVectorEqual", "EOpVectorNotEqual", "EOpLessThan", "EOpGreaterThan", "EOpLessThanEqual", "EOpGreaterThanEqual", "EOpComma", "EOpVectorTimesScalar", "EOpVectorTimesMatrix", "EOpMatrixTimesVector", "EOpMatrixTimesScalar", "EOpLogicalOr", "EOpLogicalXor", "EOpLogicalAnd", "EOpIndexDirect", "EOpIndexIndirect", "EOpIndexDirectStruct", "EOpVectorSwizzle", "EOpMethod", "EOpScoping", "EOpRadians", "EOpDegrees", "EOpSin", "EOpCos", "EOpTan", "EOpAsin", "EOpAcos", "EOpAtan", "EOpSinh", "EOpCosh", "EOpTanh", "EOpAsinh", "EOpAcosh", "EOpAtanh", "EOpPow", "EOpExp", "EOpLog", "EOpExp2", "EOpLog2", "EOpSqrt", "EOpInverseSqrt", "EOpAbs", "EOpSign", "EOpFloor", "EOpTrunc", "EOpRound", "EOpRoundEven", "EOpCeil", "EOpFract", "EOpModf", "EOpMin", "EOpMax", "EOpClamp", "EOpMix", "EOpStep", "EOpSmoothStep", "EOpIsNan", "EOpIsInf", "EOpFma", "EOpFrexp", "EOpLdexp", "EOpFloatBitsToInt", "EOpFloatBitsToUint", "EOpIntBitsToFloat", "EOpUintBitsToFloat", "EOpDoubleBitsToInt64", "EOpDoubleBitsToUint64", "EOpInt64BitsToDouble", "EOpUint64BitsToDouble", "EOpFloat16BitsToInt16", "EOpFloat16BitsToUint16", "EOpInt16BitsToFloat16", "EOpUint16BitsToFloat16", "EOpPackSnorm2x16", "EOpUnpackSnorm2x16", "EOpPackUnorm2x16", "EOpUnpackUnorm2x16", "EOpPackSnorm4x8", "EOpUnpackSnorm4x8", "EOpPackUnorm4x8", "EOpUnpackUnorm4x8", "EOpPackHalf2x16", "EOpUnpackHalf2x16", "EOpPackDouble2x32", "EOpUnpackDouble2x32", "EOpPackInt2x32", "EOpUnpackInt2x32", "EOpPackUint2x32", "EOpUnpackUint2x32", "EOpPackFloat2x16", "EOpUnpackFloat2x16", "EOpPackInt2x16", "EOpUnpackInt2x16", "EOpPackUint2x16", "EOpUnpackUint2x16", "EOpPackInt4x16", "EOpUnpackInt4x16", "EOpPackUint4x16", "EOpUnpackUint4x16", "EOpPack16", "EOpPack32", "EOpPack64", "EOpUnpack32", "EOpUnpack16", "EOpUnpack8", "EOpLength", "EOpDistance", "EOpDot", "EOpCross", "EOpNormalize", "EOpFaceForward", "EOpReflect", "EOpRefract", "EOpMin3", "EOpMax3", "EOpMid3", "EOpDPdx", "EOpDPdy", "EOpFwidth", "EOpDPdxFine", "EOpDPdyFine", "EOpFwidthFine", "EOpDPdxCoarse", "EOpDPdyCoarse", "EOpFwidthCoarse", "EOpInterpolateAtCentroid", "EOpInterpolateAtSample", "EOpInterpolateAtOffset", "EOpInterpolateAtVertex", "EOpMatrixTimesMatrix", "EOpOuterProduct", "EOpDeterminant", "EOpMatrixInverse", "EOpTranspose", "EOpFtransform", "EOpNoise", "EOpEmitVertex", "EOpEndPrimitive", "EOpEmitStreamVertex", "EOpEndStreamPrimitive", "EOpBarrier", "EOpMemoryBarrier", "EOpMemoryBarrierAtomicCounter", "EOpMemoryBarrierBuffer", "EOpMemoryBarrierImage", "EOpMemoryBarrierShared", "EOpGroupMemoryBarrier", "EOpBallot", "EOpReadInvocation", "EOpReadFirstInvocation", "EOpAnyInvocation", "EOpAllInvocations", "EOpAllInvocationsEqual", "EOpSubgroupGuardStart", "EOpSubgroupBarrier", "EOpSubgroupMemoryBarrier", "EOpSubgroupMemoryBarrierBuffer", "EOpSubgroupMemoryBarrierImage", "EOpSubgroupMemoryBarrierShared", "EOpSubgroupElect", "EOpSubgroupAll", "EOpSubgroupAny", "EOpSubgroupAllEqual", "EOpSubgroupBroadcast", "EOpSubgroupBroadcastFirst", "EOpSubgroupBallot", "EOpSubgroupInverseBallot", "EOpSubgroupBallotBitExtract", "EOpSubgroupBallotBitCount", "EOpSubgroupBallotInclusiveBitCount", "EOpSubgroupBallotExclusiveBitCount", "EOpSubgroupBallotFindLSB", "EOpSubgroupBallotFindMSB", "EOpSubgroupShuffle", "EOpSubgroupShuffleXor", "EOpSubgroupShuffleUp", "EOpSubgroupShuffleDown", "EOpSubgroupAdd", "EOpSubgroupMul", "EOpSubgroupMin", "EOpSubgroupMax", "EOpSubgroupAnd", "EOpSubgroupOr", "EOpSubgroupXor", "EOpSubgroupInclusiveAdd", "EOpSubgroupInclusiveMul", "EOpSubgroupInclusiveMin", "EOpSubgroupInclusiveMax", "EOpSubgroupInclusiveAnd", "EOpSubgroupInclusiveOr", "EOpSubgroupInclusiveXor", "EOpSubgroupExclusiveAdd", "EOpSubgroupExclusiveMul", "EOpSubgroupExclusiveMin", "EOpSubgroupExclusiveMax", "EOpSubgroupExclusiveAnd", "EOpSubgroupExclusiveOr", "EOpSubgroupExclusiveXor", "EOpSubgroupClusteredAdd", "EOpSubgroupClusteredMul", "EOpSubgroupClusteredMin", "EOpSubgroupClusteredMax", "EOpSubgroupClusteredAnd", "EOpSubgroupClusteredOr", "EOpSubgroupClusteredXor", "EOpSubgroupQuadBroadcast", "EOpSubgroupQuadSwapHorizontal", "EOpSubgroupQuadSwapVertical", "EOpSubgroupQuadSwapDiagonal", "EOpSubgroupPartition", "EOpSubgroupPartitionedAdd", "EOpSubgroupPartitionedMul", "EOpSubgroupPartitionedMin", "EOpSubgroupPartitionedMax", "EOpSubgroupPartitionedAnd", "EOpSubgroupPartitionedOr", "EOpSubgroupPartitionedXor", "EOpSubgroupPartitionedInclusiveAdd", "EOpSubgroupPartitionedInclusiveMul", "EOpSubgroupPartitionedInclusiveMin", "EOpSubgroupPartitionedInclusiveMax", "EOpSubgroupPartitionedInclusiveAnd", "EOpSubgroupPartitionedInclusiveOr", "EOpSubgroupPartitionedInclusiveXor", "EOpSubgroupPartitionedExclusiveAdd", "EOpSubgroupPartitionedExclusiveMul", "EOpSubgroupPartitionedExclusiveMin", "EOpSubgroupPartitionedExclusiveMax", "EOpSubgroupPartitionedExclusiveAnd", "EOpSubgroupPartitionedExclusiveOr", "EOpSubgroupPartitionedExclusiveXor", "EOpSubgroupGuardStop", "EOpMinInvocations", "EOpMaxInvocations", "EOpAddInvocations", "EOpMinInvocationsNonUniform", "EOpMaxInvocationsNonUniform", "EOpAddInvocationsNonUniform", "EOpMinInvocationsInclusiveScan", "EOpMaxInvocationsInclusiveScan", "EOpAddInvocationsInclusiveScan", "EOpMinInvocationsInclusiveScanNonUniform", "EOpMaxInvocationsInclusiveScanNonUniform", "EOpAddInvocationsInclusiveScanNonUniform", "EOpMinInvocationsExclusiveScan", "EOpMaxInvocationsExclusiveScan", "EOpAddInvocationsExclusiveScan", "EOpMinInvocationsExclusiveScanNonUniform", "EOpMaxInvocationsExclusiveScanNonUniform", "EOpAddInvocationsExclusiveScanNonUniform", "EOpSwizzleInvocations", "EOpSwizzleInvocationsMasked", "EOpWriteInvocation", "EOpMbcnt", "EOpCubeFaceIndex", "EOpCubeFaceCoord", "EOpTime", "EOpAtomicAdd", "EOpAtomicMin", "EOpAtomicMax", "EOpAtomicAnd", "EOpAtomicOr", "EOpAtomicXor", "EOpAtomicExchange", "EOpAtomicCompSwap", "EOpAtomicLoad", "EOpAtomicStore", "EOpAtomicCounterIncrement", "EOpAtomicCounterDecrement", "EOpAtomicCounter", "EOpAtomicCounterAdd", "EOpAtomicCounterSubtract", "EOpAtomicCounterMin", "EOpAtomicCounterMax", "EOpAtomicCounterAnd", "EOpAtomicCounterOr", "EOpAtomicCounterXor", "EOpAtomicCounterExchange", "EOpAtomicCounterCompSwap", "EOpAny", "EOpAll", "EOpCooperativeMatrixLoad", "EOpCooperativeMatrixStore", "EOpCooperativeMatrixMulAdd", "EOpBeginInvocationInterlock", "EOpEndInvocationInterlock", "EOpIsHelperInvocation", "EOpDebugPrintf", "EOpKill", "EOpTerminateInvocation", "EOpDemote", "EOpTerminateRayKHR", "EOpIgnoreIntersectionKHR", "EOpReturn", "EOpBreak", "EOpContinue", "EOpCase", "EOpDefault", "EOpConstructGuardStart", "EOpConstructInt", "EOpConstructUint", "EOpConstructInt8", "EOpConstructUint8", "EOpConstructInt16", "EOpConstructUint16", "EOpConstructInt64", "EOpConstructUint64", "EOpConstructBool", "EOpConstructFloat", "EOpConstructDouble", "EOpConstructVec2", "EOpConstructVec3", "EOpConstructVec4", "EOpConstructMat2x2", "EOpConstructMat2x3", "EOpConstructMat2x4", "EOpConstructMat3x2", "EOpConstructMat3x3", "EOpConstructMat3x4", "EOpConstructMat4x2", "EOpConstructMat4x3", "EOpConstructMat4x4", "EOpConstructDVec2", "EOpConstructDVec3", "EOpConstructDVec4", "EOpConstructBVec2", "EOpConstructBVec3", "EOpConstructBVec4", "EOpConstructI8Vec2", "EOpConstructI8Vec3", "EOpConstructI8Vec4", "EOpConstructU8Vec2", "EOpConstructU8Vec3", "EOpConstructU8Vec4", "EOpConstructI16Vec2", "EOpConstructI16Vec3", "EOpConstructI16Vec4", "EOpConstructU16Vec2", "EOpConstructU16Vec3", "EOpConstructU16Vec4", "EOpConstructIVec2", "EOpConstructIVec3", "EOpConstructIVec4", "EOpConstructUVec2", "EOpConstructUVec3", "EOpConstructUVec4", "EOpConstructI64Vec2", "EOpConstructI64Vec3", "EOpConstructI64Vec4", "EOpConstructU64Vec2", "EOpConstructU64Vec3", "EOpConstructU64Vec4", "EOpConstructDMat2x2", "EOpConstructDMat2x3", "EOpConstructDMat2x4", "EOpConstructDMat3x2", "EOpConstructDMat3x3", "EOpConstructDMat3x4", "EOpConstructDMat4x2", "EOpConstructDMat4x3", "EOpConstructDMat4x4", "EOpConstructIMat2x2", "EOpConstructIMat2x3", "EOpConstructIMat2x4", "EOpConstructIMat3x2", "EOpConstructIMat3x3", "EOpConstructIMat3x4", "EOpConstructIMat4x2", "EOpConstructIMat4x3", "EOpConstructIMat4x4", "EOpConstructUMat2x2", "EOpConstructUMat2x3", "EOpConstructUMat2x4", "EOpConstructUMat3x2", "EOpConstructUMat3x3", "EOpConstructUMat3x4", "EOpConstructUMat4x2", "EOpConstructUMat4x3", "EOpConstructUMat4x4", "EOpConstructBMat2x2", "EOpConstructBMat2x3", "EOpConstructBMat2x4", "EOpConstructBMat3x2", "EOpConstructBMat3x3", "EOpConstructBMat3x4", "EOpConstructBMat4x2", "EOpConstructBMat4x3", "EOpConstructBMat4x4", "EOpConstructFloat16", "EOpConstructF16Vec2", "EOpConstructF16Vec3", "EOpConstructF16Vec4", "EOpConstructF16Mat2x2", "EOpConstructF16Mat2x3", "EOpConstructF16Mat2x4", "EOpConstructF16Mat3x2", "EOpConstructF16Mat3x3", "EOpConstructF16Mat3x4", "EOpConstructF16Mat4x2", "EOpConstructF16Mat4x3", "EOpConstructF16Mat4x4", "EOpConstructStruct", "EOpConstructTextureSampler", "EOpConstructNonuniform", "EOpConstructReference", "EOpConstructCooperativeMatrix", "EOpConstructAccStruct", "EOpConstructGuardEnd", "EOpAssign", "EOpAddAssign", "EOpSubAssign", "EOpMulAssign", "EOpVectorTimesMatrixAssign", "EOpVectorTimesScalarAssign", "EOpMatrixTimesScalarAssign", "EOpMatrixTimesMatrixAssign", "EOpDivAssign", "EOpModAssign", "EOpAndAssign", "EOpInclusiveOrAssign", "EOpExclusiveOrAssign", "EOpLeftShiftAssign", "EOpRightShiftAssign", "EOpArrayLength", "EOpImageGuardBegin", "EOpImageQuerySize", "EOpImageQuerySamples", "EOpImageLoad", "EOpImageStore", "EOpImageLoadLod", "EOpImageStoreLod", "EOpImageAtomicAdd", "EOpImageAtomicMin", "EOpImageAtomicMax", "EOpImageAtomicAnd", "EOpImageAtomicOr", "EOpImageAtomicXor", "EOpImageAtomicExchange", "EOpImageAtomicCompSwap", "EOpImageAtomicLoad", "EOpImageAtomicStore", "EOpSubpassLoad", "EOpSubpassLoadMS", "EOpSparseImageLoad", "EOpSparseImageLoadLod", "EOpImageGuardEnd", "EOpTextureGuardBegin", "EOpTextureQuerySize", "EOpTextureQueryLod", "EOpTextureQueryLevels", "EOpTextureQuerySamples", "EOpSamplingGuardBegin", "EOpTexture", "EOpTextureProj", "EOpTextureLod", "EOpTextureOffset", "EOpTextureFetch", "EOpTextureFetchOffset", "EOpTextureProjOffset", "EOpTextureLodOffset", "EOpTextureProjLod", "EOpTextureProjLodOffset", "EOpTextureGrad", "EOpTextureGradOffset", "EOpTextureProjGrad", "EOpTextureProjGradOffset", "EOpTextureGather", "EOpTextureGatherOffset", "EOpTextureGatherOffsets", "EOpTextureClamp", "EOpTextureOffsetClamp", "EOpTextureGradClamp", "EOpTextureGradOffsetClamp", "EOpTextureGatherLod", "EOpTextureGatherLodOffset", "EOpTextureGatherLodOffsets", "EOpFragmentMaskFetch", "EOpFragmentFetch", "EOpSparseTextureGuardBegin", "EOpSparseTexture", "EOpSparseTextureLod", "EOpSparseTextureOffset", "EOpSparseTextureFetch", "EOpSparseTextureFetchOffset", "EOpSparseTextureLodOffset", "EOpSparseTextureGrad", "EOpSparseTextureGradOffset", "EOpSparseTextureGather", "EOpSparseTextureGatherOffset", "EOpSparseTextureGatherOffsets", "EOpSparseTexelsResident", "EOpSparseTextureClamp", "EOpSparseTextureOffsetClamp", "EOpSparseTextureGradClamp", "EOpSparseTextureGradOffsetClamp", "EOpSparseTextureGatherLod", "EOpSparseTextureGatherLodOffset", "EOpSparseTextureGatherLodOffsets", "EOpSparseTextureGuardEnd", "EOpImageFootprintGuardBegin", "EOpImageSampleFootprintNV", "EOpImageSampleFootprintClampNV", "EOpImageSampleFootprintLodNV", "EOpImageSampleFootprintGradNV", "EOpImageSampleFootprintGradClampNV", "EOpImageFootprintGuardEnd", "EOpSamplingGuardEnd", "EOpTextureGuardEnd", "EOpAddCarry", "EOpSubBorrow", "EOpUMulExtended", "EOpIMulExtended", "EOpBitfieldExtract", "EOpBitfieldInsert", "EOpBitFieldReverse", "EOpBitCount", "EOpFindLSB", "EOpFindMSB", "EOpCountLeadingZeros", "EOpCountTrailingZeros", "EOpAbsDifference", "EOpAddSaturate", "EOpSubSaturate", "EOpAverage", "EOpAverageRounded", "EOpMul32x16", "EOpTraceNV", "EOpTraceKHR", "EOpReportIntersection", "EOpIgnoreIntersectionNV", "EOpTerminateRayNV", "EOpExecuteCallableNV", "EOpExecuteCallableKHR", "EOpWritePackedPrimitiveIndices4x8NV", "EOpRayQueryInitialize", "EOpRayQueryTerminate", "EOpRayQueryGenerateIntersection", "EOpRayQueryConfirmIntersection", "EOpRayQueryProceed", "EOpRayQueryGetIntersectionType", "EOpRayQueryGetRayTMin", "EOpRayQueryGetRayFlags", "EOpRayQueryGetIntersectionT", "EOpRayQueryGetIntersectionInstanceCustomIndex", "EOpRayQueryGetIntersectionInstanceId", "EOpRayQueryGetIntersectionInstanceShaderBindingTableRecordOffset", "EOpRayQueryGetIntersectionGeometryIndex", "EOpRayQueryGetIntersectionPrimitiveIndex", "EOpRayQueryGetIntersectionBarycentrics", "EOpRayQueryGetIntersectionFrontFace", "EOpRayQueryGetIntersectionCandidateAABBOpaque", "EOpRayQueryGetIntersectionObjectRayDirection", "EOpRayQueryGetIntersectionObjectRayOrigin", "EOpRayQueryGetWorldRayDirection", "EOpRayQueryGetWorldRayOrigin", "EOpRayQueryGetIntersectionObjectToWorld", "EOpRayQueryGetIntersectionWorldToObject", "EOpClip", "EOpIsFinite", "EOpLog10", "EOpRcp", "EOpSaturate", "EOpSinCos", "EOpGenMul", "EOpDst", "EOpInterlockedAdd", "EOpInterlockedAnd", "EOpInterlockedCompareExchange", "EOpInterlockedCompareStore", "EOpInterlockedExchange", "EOpInterlockedMax", "EOpInterlockedMin", "EOpInterlockedOr", "EOpInterlockedXor", "EOpAllMemoryBarrierWithGroupSync", "EOpDeviceMemoryBarrier", "EOpDeviceMemoryBarrierWithGroupSync", "EOpWorkgroupMemoryBarrier", "EOpWorkgroupMemoryBarrierWithGroupSync", "EOpEvaluateAttributeSnapped", "EOpF32tof16", "EOpF16tof32", "EOpLit", "EOpTextureBias", "EOpAsDouble", "EOpD3DCOLORtoUBYTE4", "EOpMethodSample", "EOpMethodSampleBias", "EOpMethodSampleCmp", "EOpMethodSampleCmpLevelZero", "EOpMethodSampleGrad", "EOpMethodSampleLevel", "EOpMethodLoad", "EOpMethodGetDimensions", "EOpMethodGetSamplePosition", "EOpMethodGather", "EOpMethodCalculateLevelOfDetail", "EOpMethodCalculateLevelOfDetailUnclamped", "EOpMethodLoad2", "EOpMethodLoad3", "EOpMethodLoad4", "EOpMethodStore", "EOpMethodStore2", "EOpMethodStore3", "EOpMethodStore4", "EOpMethodIncrementCounter", "EOpMethodDecrementCounter", "EOpMethodConsume", "EOpMethodGatherRed", "EOpMethodGatherGreen", "EOpMethodGatherBlue", "EOpMethodGatherAlpha", "EOpMethodGatherCmp", "EOpMethodGatherCmpRed", "EOpMethodGatherCmpGreen", "EOpMethodGatherCmpBlue", "EOpMethodGatherCmpAlpha", "EOpMethodAppend", "EOpMethodRestartStrip", "EOpMatrixSwizzle", "EOpWaveGetLaneCount", "EOpWaveGetLaneIndex", "EOpWaveActiveCountBits", "EOpWavePrefixCountBits", "EOpReadClockSubgroupKHR", "EOpReadClockDeviceKHR" };

bool SpirverAstAnalyzerTraverser::visitBinary(glslang::TVisit, glslang::TIntermBinary* node)
{
	glslang::TOperator op = node->getOp();
	AddToOpCounts(op);

	switch (op)
	{
	case glslang::TOperator::EOpAssign:
		stat.stats[assign]++;
		break;
	case glslang::TOperator::EOpIndexDirect:
	case glslang::TOperator::EOpIndexIndirect:
	case glslang::TOperator::EOpIndexDirectStruct:
	case glslang::TOperator::EOpVectorSwizzle:
		stat.stats[access]++;
		break;
	case glslang::TOperator::EOpEqual:
	case glslang::TOperator::EOpNotEqual:
	case glslang::TOperator::EOpVectorEqual:
	case glslang::TOperator::EOpVectorNotEqual:
	case glslang::TOperator::EOpLessThan:
	case glslang::TOperator::EOpGreaterThan:
	case glslang::TOperator::EOpLessThanEqual:
	case glslang::TOperator::EOpGreaterThanEqual:
		stat.stats[compare]++;
		break;
	case glslang::TOperator::EOpAdd:
	case glslang::TOperator::EOpSub:
	case glslang::TOperator::EOpMul:
	case glslang::TOperator::EOpDiv:
	case glslang::TOperator::EOpMod:
	case glslang::TOperator::EOpRightShift:
	case glslang::TOperator::EOpLeftShift:
	case glslang::TOperator::EOpInclusiveOr:
	case glslang::TOperator::EOpExclusiveOr:
	case glslang::TOperator::EOpVectorTimesScalar:
	case glslang::TOperator::EOpVectorTimesMatrix:
	case glslang::TOperator::EOpMatrixTimesVector:
	case glslang::TOperator::EOpMatrixTimesScalar:
	case glslang::TOperator::EOpMatrixTimesMatrix:
	case glslang::TOperator::EOpLogicalOr:
	case glslang::TOperator::EOpLogicalXor:
	case glslang::TOperator::EOpLogicalAnd:
		stat.stats[math]++;
		break;
	default:
		stat.stats[math]++;
		unknownOperators.insert(op);
		break;
	}
	return true;
}

bool SpirverAstAnalyzerTraverser::visitUnary(glslang::TVisit, glslang::TIntermUnary* node)
{
	glslang::TOperator op = node->getOp();
	AddToOpCounts(op);

	switch (op)
	{
	case glslang::TOperator::EOpRadians:
	case glslang::TOperator::EOpDegrees:
	case glslang::TOperator::EOpAbs:
	case glslang::TOperator::EOpSign:
	case glslang::TOperator::EOpFloor:
	case glslang::TOperator::EOpTrunc:
	case glslang::TOperator::EOpRound:
	case glslang::TOperator::EOpRoundEven:
	case glslang::TOperator::EOpCeil:
	case glslang::TOperator::EOpFract:
	case glslang::TOperator::EOpModf:
	case glslang::TOperator::EOpFma:
	case glslang::TOperator::EOpFrexp:
	case glslang::TOperator::EOpLdexp:
	case glslang::TOperator::EOpNegative:
	case glslang::TOperator::EOpLogicalNot:
	case glslang::TOperator::EOpVectorLogicalNot:
	case glslang::TOperator::EOpBitwiseNot:
	case glslang::TOperator::EOpPostIncrement:
	case glslang::TOperator::EOpPostDecrement:
	case glslang::TOperator::EOpPreIncrement:
	case glslang::TOperator::EOpPreDecrement:
		stat.stats[math]++;
		break;
	case glslang::TOperator::EOpSin:
	case glslang::TOperator::EOpCos:
	case glslang::TOperator::EOpTan:
	case glslang::TOperator::EOpAsin:
	case glslang::TOperator::EOpAcos:
	case glslang::TOperator::EOpAtan:
	case glslang::TOperator::EOpSinh:
	case glslang::TOperator::EOpCosh:
	case glslang::TOperator::EOpTanh:
	case glslang::TOperator::EOpAsinh:
	case glslang::TOperator::EOpAcosh:
	case glslang::TOperator::EOpAtanh:
	case glslang::TOperator::EOpPow:
	case glslang::TOperator::EOpExp:
	case glslang::TOperator::EOpLog:
	case glslang::TOperator::EOpExp2:
	case glslang::TOperator::EOpLog2:
	case glslang::TOperator::EOpSqrt:
	case glslang::TOperator::EOpInverseSqrt:
	case glslang::TOperator::EOpLength:
	case glslang::TOperator::EOpDistance:
	case glslang::TOperator::EOpDot:
	case glslang::TOperator::EOpCross:
	case glslang::TOperator::EOpNormalize:
	case glslang::TOperator::EOpFaceForward:
	case glslang::TOperator::EOpReflect:
	case glslang::TOperator::EOpRefract:
	case glslang::TOperator::EOpOuterProduct:
	case glslang::TOperator::EOpDeterminant:
	case glslang::TOperator::EOpMatrixInverse:
	case glslang::TOperator::EOpTranspose:
		stat.stats[mathExpensive]++;
		break;
	case glslang::TOperator::EOpIsNan:
	case glslang::TOperator::EOpIsInf:
		stat.stats[compare]++;
		break;
	case glslang::TOperator::EOpCopyObject:
		stat.stats[assign]++;
		break;
	default:
		stat.stats[math]++;
		unknownOperators.insert(op);
		break;
	}
	return true;
}

bool SpirverAstAnalyzerTraverser::visitSelection(glslang::TVisit, glslang::TIntermSelection* node)
{
	//glslang::TOperator op = node->getAsOperator()->getOp(); // this is null

	stat.stats[conditional]++;
	return true;
}

bool SpirverAstAnalyzerTraverser::visitAggregate(glslang::TVisit, glslang::TIntermAggregate* node)
{
	// glslang::TString name = node->getName();
	// auto a = node->getCompleteString();
	glslang::TOperator op = node->getOp();
	AddToOpCounts(op);

	switch (op)
	{
	case glslang::TOperator::EOpSequence:
	case glslang::TOperator::EOpLinkerObjects:
		stat.stats[tempVar]++;
		break;
	case glslang::TOperator::EOpFunctionCall:
		stat.stats[functionCall]++;
		break;
	case glslang::TOperator::EOpFunction:
		stat.stats[functionDef]++;
		break;
	case glslang::TOperator::EOpClamp:
	case glslang::TOperator::EOpMin:
	case glslang::TOperator::EOpMax:
	case glslang::TOperator::EOpMix:
	case glslang::TOperator::EOpStep:
	case glslang::TOperator::EOpSmoothStep:
	case glslang::TOperator::EOpMin3:
	case glslang::TOperator::EOpMax3:
	case glslang::TOperator::EOpMid3:
		stat.stats[math]++;
		break;
	default:
		unknownOperators.insert(op);
		stat.stats[math]++;
		break;
	}
	return true;
}

bool SpirverAstAnalyzerTraverser::visitLoop(glslang::TVisit, glslang::TIntermLoop* node)
{
	//glslang::TOperator op = node->getAsOperator()->getOp(); // this is null
	
	stat.stats[loop]++;
	return true;
}

bool SpirverAstAnalyzerTraverser::visitBranch(glslang::TVisit, glslang::TIntermBranch* node)
{
	AddToOpCounts(node->getFlowOp());
	//glslang::TOperator op = node->getAsOperator()->getOp(); // this is null

	stat.stats[flow]++;
	return true;
}

bool SpirverAstAnalyzerTraverser::visitSwitch(glslang::TVisit, glslang::TIntermSwitch* node)
{
	//glslang::TOperator op = node->getAsOperator()->getOp(); // this is null

	stat.stats[switches]++;
	return true;
}


