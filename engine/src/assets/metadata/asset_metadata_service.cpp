#include <iostream>

#include "engine/assets/metadata/asset_metadata_service.h"

#include "engine/assets/asset_helpers.h"

std::unordered_map<UUID, AssetMetadata> AssetMetadataService::LoadAssetMetadata(
	const std::filesystem::path& assetDirectory) {
	std::unordered_map<UUID, AssetMetadata> assetMetadatas;

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
		if (extension == assetMetadataExtension || extension == assetUuidExtension) {
			continue;
		}

		std::cout << "Found asset file: " << filePath.string() << std::endl;

		std::filesystem::path metadataFilePath = GenerateMetadataFilePath(filePath);
		std::filesystem::path uuidFilePath = GenerateUUIDFilePath(filePath);

		if (std::filesystem::exists(metadataFilePath) && std::filesystem::exists(uuidFilePath)) {
			try {
				AssetMetadata metadata = ReadMetadataAndUUID(filePath);
				ValidateMetadataAndUUID(metadata, filePath);
				assetMetadatas.emplace(metadata.id, metadata);
			} catch (const std::runtime_error& e) {
				std::cout << "Warning: Failed to read metadata for asset, regenerating: "
						  << filePath.string() << ". " << e.what() << std::endl;
			}
		} else {
			try {
				AssetMetadata metadata = GenerateMetadata(filePath);
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

AssetMetadata AssetMetadataService::GenerateMetadata(const std::filesystem::path& assetPath) {
	AssetMetadata metadata;
	metadata.id = UUIDGenerator::GenerateUUID();
	metadata.path = assetPath;
	try {
		std::string extension = assetPath.extension().string();
		metadata.type = GetStringFromAssetType(GetAssetTypeFromExtension(extension));
		metadata.importer = "DEPRECIATED";
	} catch (const std::runtime_error& e) {
		throw std::runtime_error(
			"Failed to generate metadata for asset: " + assetPath.string() + ". " + e.what());
	}

	return metadata;
}

AssetMetadata AssetMetadataService::ReadMetadataAndUUID(const std::filesystem::path& assetPath) {
	AssetMetadata metadata;
	std::filesystem::path metadataFilePath = GenerateMetadataFilePath(assetPath);
	std::filesystem::path uuidFilePath = GenerateUUIDFilePath(assetPath);
	try {
		metadataSerializer.Read(metadata, metadataFilePath);
		uuidSerializer.Read(metadata.id, uuidFilePath);
	} catch (const std::runtime_error& e) {
		throw std::runtime_error(
			"Failed to parse metadata from file: " + metadataFilePath.string() + ". " + e.what());
	}
	return metadata;
}

void AssetMetadataService::ValidateMetadataAndUUID(
	AssetMetadata& metadata,
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

void AssetMetadataService::WriteMetadataAndUUID(
	const AssetMetadata& metadata,
	const std::filesystem::path& assetPath) {
	std::filesystem::path metadataFilePath = GenerateMetadataFilePath(assetPath);
	std::filesystem::path uuidFilePath = GenerateUUIDFilePath(assetPath);
	try {
		metadataSerializer.Write(metadata, metadataFilePath);
		uuidSerializer.Write(metadata.id, uuidFilePath);
	} catch (const std::runtime_error& e) {
		throw std::runtime_error(
			"Failed to write metadata to file: " + metadataFilePath.string() + ". " + e.what());
	}
}

std::filesystem::path AssetMetadataService::GenerateMetadataFilePath(
	const std::filesystem::path& assetPath) const {
	std::filesystem::path metadataFilePath = assetPath;
	metadataFilePath += assetMetadataExtension;
	return metadataFilePath;
}

std::filesystem::path AssetMetadataService::GenerateUUIDFilePath(
	const std::filesystem::path& assetPath) const {
	std::filesystem::path uuidFilePath = assetPath;
	uuidFilePath += assetUuidExtension;
	return uuidFilePath;
}
