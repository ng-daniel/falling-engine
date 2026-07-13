#ifndef ENGINE_ASSETS_METADATA_ASSET_METADATA_SERVICE_H
#define ENGINE_ASSETS_METADATA_ASSET_METADATA_SERVICE_H

#include <filesystem>
#include <string>
#include <unordered_map>

#include "engine/assets/asset_data.h"
#include "engine/serialization/asset_metadata_serializer.h"
#include "engine/serialization/asset_uuid_serializer.h"

/**
 * @brief Service for managing asset metadata.
 * 
 * This service provides functionality to load, validate, and write asset metadata.
 */
class AssetMetadataService {
public:
	std::unordered_map<UUID, AssetMetadata> LoadAssetMetadata(const std::filesystem::path& assetDirectory);

private:
	const std::string assetUuidExtension = ".fid";
	const std::string assetMetadataExtension = ".fmeta";

	AssetMetadataSerializer metadataSerializer;
	AssetUUIDSerializer uuidSerializer;

	AssetMetadata GenerateMetadata(const std::filesystem::path& assetPath);
	AssetMetadata ReadMetadataAndUUID(const std::filesystem::path& assetPath);
	void ValidateMetadataAndUUID(AssetMetadata& metadata, const std::filesystem::path& assetPath);
	void WriteMetadataAndUUID(const AssetMetadata& metadata, const std::filesystem::path& assetPath);
	std::filesystem::path GenerateMetadataFilePath(const std::filesystem::path& assetPath) const;
	std::filesystem::path GenerateUUIDFilePath(const std::filesystem::path& assetPath) const;
};

#endif // ENGINE_ASSETS_METADATA_ASSET_METADATA_SERVICE_H
