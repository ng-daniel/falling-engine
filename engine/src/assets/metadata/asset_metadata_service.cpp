#include <iostream>

#include "engine/assets/metadata/asset_metadata_service.h"

#include "engine/assets/asset_helpers.h"

/**
 * @brief Loads asset metadata from the specified asset directory.
 * @param assetDirectory The directory to scan for assets and their metadata.
 * @return A map of UUIDs to AssetMetadata objects for all assets found in the directory.
 * @throws std::runtime_error if the asset directory does not exist or if there are issues reading metadata files.
 * @details
 * Ok basically this thing takes the root asset directory and walks it completely.
 * Skipping over any non-file files, and skipping ID and METADATA files.
 * For any regular file, tries to read or generate metadata based on the file extension.
 */
std::unordered_map<UUID, SourceAssetMetadata> AssetMetadataService::LoadAssetMetadata(
	const std::filesystem::path& assetDirectory) {
	
	std::unordered_map<UUID, SourceAssetMetadata> assetMetadatas;
	std::cout << "Processing asset directory: " << assetDirectory.string() << std::endl;

	if (!std::filesystem::exists(assetDirectory)) {
		throw std::runtime_error("Asset directory does not exist: " + assetDirectory.string());
	}

	for (const auto& entry : std::filesystem::recursive_directory_iterator(assetDirectory)) {
		if (!entry.is_regular_file()) {
			continue;
		}

		const std::filesystem::path& filePath = entry.path();
		const std::string extension = filePath.extension().string();
		if (extension == assetMetadataExtension) {
			continue;
		}

		std::cout << "Found asset file: " << filePath.string() << std::endl;

		std::filesystem::path metadataFilePath = GenerateMetadataFilePath(filePath);

		if (std::filesystem::exists(metadataFilePath)) {
			try {
				SourceAssetMetadata metadata = ReadMetadataAndUUID(filePath);
				ValidateMetadataAndUUID(metadata, filePath);
				assetMetadatas.emplace(metadata.id, metadata);
			} catch (const std::runtime_error& e) {
				std::cout << "Warning: Failed to read metadata for asset, regenerating: "
						  << filePath.string() << ". " << e.what() << std::endl;
			}
		} else {
			try {
				SourceAssetMetadata metadata = GenerateMetadata(filePath);
				WriteMetadataAndUUID(metadata, filePath);
				assetMetadatas.emplace(metadata.id, metadata);
			} catch (const std::runtime_error& e) {
				std::cout << "Warning: Failed to process asset: "
						  << filePath.string() << ". " << e.what() << std::endl;
			}
		}
	}

	return assetMetadatas;
}

/**
 * @brief Generates metadata for the given asset path.
 * @param assetPath The path of the asset to generate metadata for.
 * @return The generated AssetMetadata object.
 * @throws std::runtime_error if metadata generation fails.
 */
SourceAssetMetadata AssetMetadataService::GenerateMetadata(const std::filesystem::path& assetPath) {
	SourceAssetMetadata metadata;
	metadata.id = UUIDGenerator::GenerateUUID();
	metadata.path = assetPath;
	try {
		std::string extension = assetPath.extension().string();
		metadata.type = GetStringFromAssetType(GetAssetTypeFromExtension(extension));
	} catch (const std::runtime_error& e) {
		throw std::runtime_error(
			"Failed to generate metadata for asset: " + assetPath.string() + ". " + e.what());
	}

	return metadata;
}

/**
 * @brief Reads metadata and UUID for the given asset path.
 * @param assetPath The path of the asset to read metadata and UUID for.
 * @return The AssetMetadata object containing the metadata and UUID.
 * @throws std::runtime_error if reading metadata or UUID fails.
 */
SourceAssetMetadata AssetMetadataService::ReadMetadataAndUUID(const std::filesystem::path& assetPath) {
	SourceAssetMetadata metadata;
	std::filesystem::path metadataFilePath = GenerateMetadataFilePath(assetPath);
	try {
		metadataSerializer.Read(metadata, metadataFilePath);
	} catch (const std::runtime_error& e) {
		throw std::runtime_error(
			"Failed to parse metadata from file: " + metadataFilePath.string() + ". " + e.what());
	}
	return metadata;
}

/**
 * @brief Validates the metadata and UUID for the given asset path.
 * @param metadata The metadata to validate.
 * @param assetPath The path of the asset to validate against.
 * @throws std::runtime_error if validation fails.
 */
void AssetMetadataService::ValidateMetadataAndUUID(
	SourceAssetMetadata& metadata,
	const std::filesystem::path& assetPath) {
	bool changed = false;

	std::string realAssetPath = assetPath.string();
	std::cout << "\tReal asset path: " << realAssetPath << std::endl;
	std::cout << "\tRecorded asset path: " << metadata.path << std::endl;
	if (realAssetPath != metadata.path) {
		std::cout << "\tWarning: Asset path in metadata file does not match actual asset path. Updating metadata path."
				  << std::endl;
		metadata.path = realAssetPath;
		changed = true;
	}

	if (changed) {
		WriteMetadataAndUUID(metadata, realAssetPath);
	}
}

/**
 * @brief Writes metadata and UUID for the given asset path.
 * @param metadata The metadata to write.
 * @param assetPath The path of the asset to write metadata and UUID for.
 * @throws std::runtime_error if writing metadata or UUID fails.
 */
void AssetMetadataService::WriteMetadataAndUUID(
	const SourceAssetMetadata& metadata,
	const std::filesystem::path& assetPath) {
	std::filesystem::path metadataFilePath = GenerateMetadataFilePath(assetPath);
	try {
		metadataSerializer.Write(metadata, metadataFilePath);
	} catch (const std::runtime_error& e) {
		throw std::runtime_error(
			"Failed to write metadata to file: " + metadataFilePath.string() + ". " + e.what());
	}
}

/**
 * @brief Generates the metadata file path for the given asset path.
 * @param assetPath The path of the asset to generate the metadata file path for.
 * @return The generated metadata file path.
 */
std::filesystem::path AssetMetadataService::GenerateMetadataFilePath(
	const std::filesystem::path& assetPath) const {
	std::filesystem::path metadataFilePath = assetPath;
	metadataFilePath += assetMetadataExtension;
	return metadataFilePath;
}