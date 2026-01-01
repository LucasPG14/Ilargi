#pragma once

#include "Events/Event.h"
#include "Events/WindowEvents.h"
#include "Events/KeyEvents.h"

namespace Ilargi
{
	class UUID;
	class Texture2D;
	class MaterialPanel;

	class ResourcesPanel
	{
	public:
		/*
		* @brief Constructor.
		*/
		ResourcesPanel();

		/*
		* @brief Destructor.
		*/
		~ResourcesPanel();

		/*
		* @brief Renders the resource panel.
		*/
		void Render();

		/*
		* @brief Manages the received event.
		* @param aEvent The event to manage.
		*/
		void OnEvent(Event& aEvent);

		/*
		* @brief Refresh the asset tree.
		*/
		void RefreshAssets();

	private:
		/*
		* @brief Manages the window drop event.
		* @param aEvent The window drop event information.
		*/
		bool OnDropEvent(WindowDropEvent& aEvent);

		/*
		* @brief Manages the key pressed event.
		* @param aEvent The key pressed event information.
		*/
		bool OnKeyPressedEvent(KeyPressedEvent& aEvent);

		/*
		* @brief Draws the directory as normal.
		*/
		void NormalDirectory();

		/*
		* @brief Draws the recursive directory.
		*/
		void RecursiveDirectory();
	private:
		std::filesystem::path mActualDir; // The actual directory.
		std::filesystem::path mSelectedFile; // The selected file.

		std::string mSearch; // The string with the keyword to search.

		std::unordered_map<std::filesystem::path, UUID> mResources; // Map with the paths and UUID of the resources.

		std::shared_ptr<Texture2D> mFolderIcon; // The folder icon.
		std::shared_ptr<Texture2D> mFileIcon; // The file icon.

		MaterialPanel* mMaterialPanel; // Instance of the material panel.

		bool mResourcesPanelFocused; // Indicates if the resources panel is focused.
	};
}