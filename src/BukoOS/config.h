#pragma once

// Configure your startup BukoOS here! 
// Just define the right macros and you are good to go!
// BUKO_SERVER_BUILD   - build WITHOUT DisplayInfo
// BUKO_PRINT_MEM_INFO - prints memory info on bootup
// BUKO_TEST_MEMORY_PAGE_MAP - (Recommended for debugging) tests if memory page map is mapped correctly
// CONFIG_NO_PROGRESSBAR     - Remove the progress bar
// Color schemes: Color schemes are a way to customize your BukoOS startup experience.
// Just choose a theme you want by defining "COLORSCHEME_{name}" or if you prefer to make your own custom one,
// define COLORSCHEME_NONE. 
//
// Built-in Color schemes:
// 
// Dark mode (DARKMODE) - an editor-like theme. Suitable for those who don't want to get their eyes blasted every
// time they boot up the OS. 
// 
// Light mode (LIGHTMODE) - its the exact oposite of Dark mode. For those who like getting flash banged every time
// they bootup the OS
//
// Tokyo night (TOKYONIGHT) - (NOTE: Credit goes to: https://github.com/folke/tokyonight.nvim) its a smooth and clean
// theme, that can make your OS really feel like Home
// 
// Peach (PEACH) - a peach theme, nothing more to say :)
//
// Wilderness (WILDERNESS) - a jungle like theme. For those who want to stay hidden in the crowns of the trees
//
// Purple dome (PURPLE_DOME) - its a nice and clean dark-purple theme. 
//
// default (No defines): Just plain old white on black. 
//



// Developers Choice :D
#if 1
//#define BUKO_PRINT_MEM_INFO
//#define BUKO_TEST_MEMORY_PAGE_MAP
#define COLORSCHEME_TOKYONIGHT
#endif
//#define BUKO_SERVER_BUILD
#ifdef COLORSCHEME_NONE
#elif defined(COLORSCHEME_DARKMODE)
#define CONFIG_PROGRESS_BAR_COLOR Pixel(34,34,34,0xFF)
#define CONFIG_BACKGROUND_COLOR Pixel(44,44,44,0xFF)
#define CONFIG_TEXT_COLOR       Pixel(230,230,230,0xFF)
#elif defined(COLORSCHEME_LIGHTMODE) 
#define CONFIG_BACKGROUND_COLOR Pixel(211,211,211,0xFF)
#define CONFIG_PROGRESS_BAR_COLOR Pixel(221,221,221,0xFF)
#define CONFIG_TEXT_COLOR       Pixel(25,25,25,0xFF)
#elif defined(COLORSCHEME_TOKYONIGHT)                      
// Inspired by https://github.com/folke/tokyonight.nvim
// And https://github.com/enkia/tokyo-night-vscode-them
// Check them out if you are interested in these cool neovim & vscode color themes <3
#define CONFIG_PROGRESS_BAR_COLOR     Pixel(26, 30, 49, 0xFF)
#define CONFIG_BACKGROUND_COLOR       Pixel(36, 40, 59,0xFF)
#define CONFIG_TEXT_COLOR             Pixel(192, 202, 245,0xFF)
#elif defined(COLORSCHEME_PEACH)
#define CONFIG_PROGRESS_BAR_COLOR     Pixel(245, 229, 175, 0xFF)
#define CONFIG_BACKGROUND_COLOR       Pixel(255, 229, 180, 0xFF)
//#define CONFIG_BACKGROUND_COLOR       Pixel(235, 199, 150, 0xFF)
#define CONFIG_TEXT_COLOR             Pixel(21, 21, 21, 0xFF)
#elif defined(COLORSCHEME_WILDERNESS) 
#define CONFIG_PROGRESS_BAR_COLOR     Pixel(8, 44, 5, 0xFF)
#define CONFIG_BACKGROUND_COLOR       Pixel(13, 49, 10,0xFF)
#define CONFIG_TEXT_COLOR             Pixel(64, 132, 58, 0xFF)
#elif defined(COLORSCHEME_PURPLE_DOME)
#define CONFIG_PROGRESS_BAR_COLOR       Pixel(7, 0, 42,0xFF)
#define CONFIG_BACKGROUND_COLOR       Pixel(12, 0, 47,0xFF)
#define CONFIG_TEXT_COLOR             Pixel(94, 70, 168, 0xFF)
#else
#define CONFIG_PROGRESS_BAR_COLOR Pixel(25, 25, 25, 0xFF)
#define CONFIG_BACKGROUND_COLOR Pixel(0,0,0,0xFF)
#define CONFIG_TEXT_COLOR       Pixel(0xFF,0xFF,0xFF,0xFF)
#endif
// Hello
