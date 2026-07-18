#ifndef ENGINE_ASSETS_METADATA_ASSET_METADATA_SERVICE_H
#define ENGINE_ASSETS_METADATA_ASSET_METADATA_SERVICE_H

#include <filesystem>
#include <string>
#include <unordered_map>

#include "engine/assets/asset_data.h"
#include "engine/serialization/asset_metadata_serializer.h"

/**
 * @brief Service for managing asset metadata.
 * 
 * This service provides functionality to load, validate, and write asset metadata.
 */
class AssetMetadataService {
public:
	std::unordered_map<UUID, SourceAssetMetadata> LoadAssetMetadata(const std::filesystem::path& assetDirectory);
	RuntimeAssetMetadata GenerateRuntimeAssetMetadata(
		UUID assetId, 
		const std::string& exportName,
		Asset::AssetType assetType,
		const SourceAssetMetadata& sourceMetadata);
	void WriteMetadataAndUUID(const SourceAssetMetadata& metadata, const std::filesystem::path& assetPath);

private:
	const std::string assetMetadataExtension = ".fmeta";

	AssetMetadataSerializer metadataSerializer;

	SourceAssetMetadata GenerateSourceMetadata(const std::filesystem::path& assetPath);

	SourceAssetMetadata ReadMetadataAndUUID(const std::filesystem::path& assetPath);
	void ValidateMetadataAndUUID(SourceAssetMetadata& metadata, const std::filesystem::path& assetPath);

	std::filesystem::path GenerateMetadataFilePath(const std::filesystem::path& assetPath) const;
};

#endif // ENGINE_ASSETS_METADATA_ASSET_METADATA_SERVICE_H
