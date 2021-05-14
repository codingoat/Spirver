#pragma once
#include <glslang/Include/intermediate.h>
#include <glslang/Include/Types.h>

namespace Spirver {

enum ShaderStatTypes {
	glslLines = 0, spirvLines = 1,
	math = 2, mathExpensive = 3, conditional = 4,
	assign = 5, access = 6, compare = 7,
	functionDef = 8, functionCall = 9,
	tempVar = 10, loop = 11, flow = 12, switches = 13,
	shaderStatTypesCount = 14
};
extern const char* shaderStatNames[14];

/// Data of different operations in a shader
struct ShaderStat
{
	unsigned int* stats = nullptr; // shaderStatTypesCount
	unsigned int* opCounts = nullptr; // 791

	ShaderStat();
	ShaderStat(const ShaderStat& o);
	ShaderStat(ShaderStat&& o);

	ShaderStat& operator=(const ShaderStat& o);
	ShaderStat& operator=(ShaderStat&& o);
	bool operator==(ShaderStat& o);
	bool operator!=(ShaderStat& o);

	~ShaderStat()
	{
		if (stats != nullptr) delete[] stats;
		if (opCounts != nullptr) delete[] opCounts;
	}
};

extern const char* glslangOperatorNames[791];

}

std::ostream& operator<<(std::ostream& os, Spirver::ShaderStat& s);

namespace Spirver::detail
{

class SpirverAstAnalyzerTraverser : public glslang::TIntermTraverser
{
public:
	bool visitBinary(glslang::TVisit, glslang::TIntermBinary* node) override;
	bool visitUnary(glslang::TVisit, glslang::TIntermUnary* node) override;
	bool visitSelection(glslang::TVisit, glslang::TIntermSelection* node) override;
	bool visitAggregate(glslang::TVisit, glslang::TIntermAggregate* node) override;
	bool visitLoop(glslang::TVisit, glslang::TIntermLoop* node) override;
	// Handle case, break, continue, return, and kill.
	bool visitBranch(glslang::TVisit, glslang::TIntermBranch* node) override;
	bool visitSwitch(glslang::TVisit, glslang::TIntermSwitch* node) override;

	/// Get the result of the analysis
	ShaderStat GetShaderStat() { return stat; }

	/// Get operators that were not recognised during basic stat creation
	std::set<glslang::TOperator> GetUnknownOperators() { return unknownOperators; }
private:
	ShaderStat stat;
	std::set<glslang::TOperator> unknownOperators;

	void AddToOpCounts(glslang::TOperator op);
};

}
