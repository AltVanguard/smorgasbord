#include "gpuapi.hpp"

#include <smorgasbord/image/image.hpp>
#include <smorgasbord/util/log.hpp>

#include <map>

using namespace Smorgasbord;

RasterizationStageFlag Smorgasbord::ParseShaderStageFlags(
	std::string stageFlagsString)
{
	RasterizationStageFlag stageFlags = RasterizationStageFlag::None;
	
	const size_t numChars = stageFlagsString.length();
	for (size_t i = 0; i < numChars; i++)
	{
		switch (stageFlagsString[i])
		{
		case 'a':
			return RasterizationStageFlag::All;
		case 'v':
			stageFlags = stageFlags | RasterizationStageFlag::Vertex;
			break;
		case 'c': // for Control
		case 'd': // for Domain
			stageFlags =
					stageFlags | RasterizationStageFlag::TesselationControl;
			break;
		case 'e': // for Evaluation
		case 'h': // for Hull
			stageFlags = stageFlags
					| RasterizationStageFlag::TesselationEvaluation;
			break;
		case 'g':
			stageFlags = stageFlags | RasterizationStageFlag::Geometry;
			break;
		case 'f':
			stageFlags = stageFlags | RasterizationStageFlag::Fragment;
			break;
			
		default:
			LogE("Not recognised stage flag");
			return RasterizationStageFlag::None;
		}
	}
	
	return stageFlags;
}

inline std::map<std::string, VariableType> InitShaderVariableTypes()
{
	std::map<std::string, VariableType> types;
	types.emplace("float", VariableType(VariableBaseType::Float, 1));
	types.emplace("vec2", VariableType(VariableBaseType::Float, 2));
	types.emplace("vec3", VariableType(VariableBaseType::Float, 3));
	types.emplace("vec4", VariableType(VariableBaseType::Float, 4));
	types.emplace("int32_t", VariableType(VariableBaseType::Int, 1));
	types.emplace("ivec2", VariableType(VariableBaseType::Int, 2));
	types.emplace("ivec3", VariableType(VariableBaseType::Int, 3));
	types.emplace("ivec4", VariableType(VariableBaseType::Int, 4));
	types.emplace("uint32_t", VariableType(VariableBaseType::UInt, 1));
	types.emplace("uvec2", VariableType(VariableBaseType::UInt, 2));
	types.emplace("uvec3", VariableType(VariableBaseType::UInt, 3));
	types.emplace("uvec4", VariableType(VariableBaseType::UInt, 4));
	types.emplace("mat2", VariableType(VariableBaseType::Matrix, 2, 2));
	types.emplace("mat3x4", VariableType(VariableBaseType::Matrix, 3, 4));
	types.emplace("mat4", VariableType(VariableBaseType::Matrix, 4, 4));
	// TODO
	return types;
}

inline std::string Trim(std::string text)
{
	size_t first = text.find_first_not_of("\r\n\t ");
	size_t last = text.find_last_not_of("\r\n\t ");
	if (first != std::string::npos)
	{
		return text.substr(first, last - first + 1);
	}
	else
	{
		return "";
	}
}

inline void Print(ParameterBuffer &parameterBuffer, std::ostream &s)
{
	std::stringstream text;
	uint8_t *minAddress = nullptr;
	uint8_t *maxAddress = nullptr;
	
	text << "Buffer modifiers: " << parameterBuffer.GetModifier() << std::endl;
	
	const std::vector<ParameterBufferField> &fields = parameterBuffer.GetFields();
	for (const ParameterBufferField &field : fields)
	{
		uint8_t *pStart = (uint8_t*)field.p;
		if (field.p < minAddress || minAddress == nullptr)
		{
			minAddress = (uint8_t*)field.p;
		}
		
		uint8_t *pEnd = &pStart[field.size];
		if (pEnd > maxAddress)
		{
			maxAddress = pEnd;
		}
		
		size_t fieldSize = (size_t)(pEnd - pStart);
		text << field.type << " " << field.name
			<< " '" << field.modifier << "' "
			<< fieldSize << std::endl;
	}
	
	s << text.str()
		<< "min: " << (size_t)minAddress << std::endl
		<< "max: " << (size_t)maxAddress << std::endl
		<< "size: " << (size_t)(maxAddress - minAddress) + 1 << std::endl;
}

void TextureSampler::ParseFilter(const std::string &text)
{
	const std::string filterParameterName = "filter";
	const size_t filterParameterNameLength =
		filterParameterName.length();
	
	size_t parameterStartPos = 0;
	size_t argumentStartPos = 0;
	size_t argumentEndPos = 0;
	parameterStartPos = text.find(filterParameterName);
	if (parameterStartPos != std::string::npos)
	{
		argumentStartPos = text.find_first_of(
			"nlcrm", parameterStartPos + filterParameterNameLength);
		if (argumentStartPos != std::string::npos)
		{
			argumentEndPos = text.find_first_not_of(
				"nlcrm", argumentStartPos);
			std::string filterString = text.substr(
				argumentStartPos,
				argumentEndPos == std::string::npos
					? text.length() - argumentStartPos
					: argumentEndPos - argumentStartPos);
			
			if (filterString.length() != 5)
			{
				LogE("Couldn't parse filter string \"{0}\"",
					filterString);
			}
			else
			{
				minify = ParseSamplerFilter(filterString[0]);
				magnify = ParseSamplerFilter(filterString[1]);
				s = ParseSamplerWrap(filterString[2]);
				t = ParseSamplerWrap(filterString[3]);
				r = ParseSamplerWrap(filterString[4]);
			}
		}
	}
}

Smorgasbord::Buffer::Buffer(
	BufferType _bufferType, 
	BufferUsageType _accessType, 
	BufferUsageFrequency _accessFrequency, 
	int _size)
	: bufferType(_bufferType)
	, accessType(_accessType)
	, accessFrequency(_accessFrequency)
	, size(_size)
{ }

Smorgasbord::Texture::Texture(glm::uvec2 _imageSize, TextureFormat _textureFormat)
	: size(_imageSize), format(_textureFormat)
{ }

const std::map<std::string, VariableType> &RasterizationShader::GetVariableTypes()
{
	static std::map<std::string, VariableType> types = InitShaderVariableTypes();
	return types;
}

RasterizationShader::RasterizationShader(std::string _name)
	: name(_name)
{ }

void Smorgasbord::RasterizationShader::SetSource(
	Smorgasbord::ResourceReference source)
{
	sources.clear();
	
	sourceFile = source;
	std::string text = sourceFile.GetTextContents();
	
	text = FilterComments(text);
	std::string mainSource = ProcessIncludes(text, sourceFile);
	
	// Extract stages
	
	static std::map<std::string, RasterizationStage> stageTypes;
	if (stageTypes.size() == 0)
	{
		stageTypes["vertex"] = RasterizationStage::Vertex;
		stageTypes["tesselation_control"] =
			RasterizationStage::TesselationControl;
		stageTypes["control"] = RasterizationStage::TesselationControl;
		stageTypes["domain"] = RasterizationStage::TesselationControl;
		stageTypes["tesselation_evaluation"] =
			RasterizationStage::TesselationEvaluation;
		stageTypes["evaluation"] =
			RasterizationStage::TesselationEvaluation;
		stageTypes["hull"] = RasterizationStage::TesselationEvaluation;
		stageTypes["geometry"] = RasterizationStage::Geometry;
		stageTypes["fragment"] = RasterizationStage::Fragment;
	}
	
	const std::string stageDirective = "##stage";
	const size_t stageDirectiveLength = stageDirective.length();
	const std::string outputDirective = "##output";
	const size_t outputDirectiveLength = outputDirective.length();
	// TODO: ##input
	///const string inputDirective = "##input";
	///const size_t inputDirectiveLength = inputDirective.length();
	
	/// Points to the first '#' of "##stage" of the current stage
	size_t segmentStartPos = 0;
	/// Points to the first '#' of "##stage" of the next stage or EOF
	size_t segmentEndPos = 0;
	/// Points to the first line break after a "##stage"
	size_t stageTypeEndPos = 0;
	/// The stage name specified after a "##stage"
	std::string stageName;
	/// Points to the first '#' of "##output" of the current stage
	size_t outputStartPos = 0;
	/// Points to the first '{' after "##output"
	size_t outputBlockStartPos = 0;
	/// Points to the first '}' after "##output"
	size_t outputBlockEndPos = 0;
	/// The name of the upcoming input interface block
	std::string outputBlockName;
	/// The content of the upcoming input interface block
	std::string outputBlock;
	
	std::map<RasterizationStage, std::stringstream> stageStreams;
	bool hadAnyStages = false;
	while (true)
	{
		segmentStartPos = mainSource.find(stageDirective, segmentEndPos);
		if (segmentStartPos == std::string::npos)
		{
			// No more stages
			
			if (!hadAnyStages)
			{
				LogE("Rasterization shader contains no stages");
				return;
			}
			
			break;
		}
		
		hadAnyStages = true;
		
		stageTypeEndPos = mainSource.find_first_of(
			"\r\n", segmentStartPos + stageDirectiveLength);
		
		/// If a ##stage line ends with EOF it's an error in the file
		if (stageTypeEndPos == std::string::npos)
		{
			LogI("Unexpected EOF. Incomplete stage directive");
			return;
		}
		
		/// Empty stage directive already handled, so no check for a
		/// possible invalid endpos
		std::string stageNameUntrimmed = mainSource.substr(
			segmentStartPos + stageDirectiveLength,
			stageTypeEndPos - (segmentStartPos + stageDirectiveLength));
		stageName = Trim(stageNameUntrimmed);
		
		auto foundStageType = stageTypes.find(stageName);
		if (foundStageType == stageTypes.end())
		{
			LogE("Unrecognised shader stage \"" + stageName + "\"");
			return;
		}
		
		RasterizationStage stageType = foundStageType->second;
		
		segmentEndPos = mainSource.find(
			stageDirective, segmentStartPos + stageDirectiveLength);
		
		/// A segment might end with EOF, and we can't do
		/// math with std::string::npos
		if (segmentEndPos == std::string::npos)
		{
			segmentEndPos = mainSource.length();
		}
		
		std::stringstream &stageSource = stageStreams[stageType];
		
		if (outputBlockName.length() > 0)
		{
			stageSource << "\n""in " << outputBlockName << " {"
				<< outputBlock << "} "
				<< (stageType == RasterizationStage::Geometry ? "a[]" : "a")
				<< ";";
		}
		
		outputBlockName = "";
		outputBlock = "";
		
		outputStartPos = mainSource.find(
			outputDirective, stageTypeEndPos);
		
		if (outputStartPos != std::string::npos && outputStartPos < segmentEndPos)
		{
			outputBlockStartPos = mainSource.find_first_of(
				"{", outputStartPos + outputDirectiveLength);
			outputBlockEndPos = mainSource.find_first_of(
				"}", outputStartPos + outputDirectiveLength);
			
			if (outputBlockEndPos == std::string::npos
				|| outputBlockEndPos >= segmentEndPos
				|| outputBlockStartPos == std::string::npos
				|| outputBlockStartPos >= outputBlockEndPos)
			{
				LogE("Error while parsing ##output block");
				return;
			}
			
			outputBlockName = stageName + "OutputBlock";
			outputBlock = mainSource.substr(
				outputBlockStartPos + 1,
				outputBlockEndPos - (outputBlockStartPos + 1));
			
			stageSource << mainSource.substr(
				stageTypeEndPos,
				outputStartPos - stageTypeEndPos);
			
			stageSource << "\n""out " << outputBlockName << " {"
				<< outputBlock << "} o;";
			
			stageSource << mainSource.substr(
				outputBlockEndPos + 1,
				segmentEndPos - (outputBlockEndPos + 1));
		}
		else
		{
			stageSource << mainSource.substr(
				stageTypeEndPos,
				segmentEndPos - stageTypeEndPos);
		}
	}
	
	for (uint32_t i = 0; i < (uint32_t)RasterizationStage::Num; i++)
	{
		RasterizationStage stageType = (RasterizationStage)i;
		auto result = stageStreams.find(stageType);
		if (result != stageStreams.end())
		{
			sources[stageType] = result->second.str();
		}
	}
}

void Smorgasbord::RasterizationShader::AddText(
	Smorgasbord::RasterizationStageFlag stageFlags,
	const std::string &text)
{
	// TODO
}

std::string RasterizationShader::FilterComments(std::string text)
{
	std::string filteredText = text;
	
	size_t lineCommentStartPos = 0, lineCommentEndPos = 0;
	while (lineCommentStartPos != std::string::npos)
	{
		lineCommentStartPos = filteredText.find("//", lineCommentEndPos);
		if (lineCommentStartPos != std::string::npos)
		{
			lineCommentEndPos = filteredText.find_first_of(
				"\r\n", lineCommentStartPos);
			filteredText = filteredText.erase(
				lineCommentStartPos, lineCommentEndPos - lineCommentStartPos);
			lineCommentEndPos = filteredText.find_first_not_of(
				"\r\n", lineCommentStartPos);
		}
	}
	
	size_t blockCommentStartPos = 0, blockCommentEndPos = 0;
	while (blockCommentStartPos != std::string::npos)
	{
		blockCommentStartPos = filteredText.find("/*", blockCommentEndPos);
		if (blockCommentStartPos != std::string::npos)
		{
			blockCommentEndPos = filteredText.find(
				"*/", blockCommentStartPos);
			filteredText = filteredText.erase(
				blockCommentStartPos,
				blockCommentEndPos - blockCommentStartPos + 2);
			blockCommentEndPos = blockCommentStartPos;
		}
	}
	
	return filteredText;
}

std::string RasterizationShader::ProcessIncludes(
	std::string text, ResourceReference &ref)
{
	const std::string includeDirective = "##include";
	const size_t inludeDirectiveLength = includeDirective.length();
	
	size_t includeStartPos = 0, includeEndPos = 0;
	while (includeStartPos != std::string::npos)
	{
		includeStartPos = text.find(includeDirective, includeEndPos);
		if (includeStartPos != std::string::npos)
		{
			// Parse quotes
			
			includeEndPos = text.find_first_of(
				"\r\n", includeStartPos + inludeDirectiveLength);
			size_t includeFilenameStartPos = text.find_first_of(
				"\"", includeStartPos + inludeDirectiveLength);
			if (includeFilenameStartPos != std::string::npos)
			{
				size_t includeFilenameEndPos = text.find_first_of(
					"\"", includeFilenameStartPos + 1);
				std::string includeFilename = text.substr(
					includeFilenameStartPos + 1,
					includeFilenameEndPos - (includeFilenameStartPos + 1));
				
				// TODO: add ref.Get(includeFilename) to dependencies so
				//		if a dependency changes, the shader can be reloaded
				
				std::string includeText =
					ref.Get(includeFilename).GetTextContents();
				
				AssertE(
					includeText.length() > 0,
					"Cannot open included shader source file \""
						+ includeFilename + "\"");
				
				text = text.replace(
					includeStartPos,
					includeEndPos - includeStartPos, includeText);
			}
		}
	}
	
	return text;
}

std::vector<std::shared_ptr<Smorgasbord::CommandBuffer>>
	Smorgasbord::Device::CreateCommandBuffers(uint32_t num)
{
	std::vector<std::shared_ptr<CommandBuffer>> commandBuffers;
	for (uint32_t i = 0; i < num; i++)
	{
		commandBuffers.emplace_back(CreateCommandBuffer());
	}
	return commandBuffers;
}
