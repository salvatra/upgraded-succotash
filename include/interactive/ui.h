/**
 * @file ui.h
 * @brief Terminal User Interface (TUI) formatting and display utilities.
 *
 * This header defines ANSI escape sequences for rich text output (colors, bolding)
 * and declares functions responsible for rendering the application's menus and
 * visual components. It separates the "View" logic from the "Controller" (Shell).
 */

#ifndef UI_H
#define UI_H

/* --- ANSI Escape Codes for Text Formatting --- */

/**
 * @name Text Attributes
 * @brief Modifiers for text styling.
 */
///@{
#define ANSI_RESET "\x1b[0m"     /**< Resets all attributes to default. */
#define ANSI_BOLD "\x1b[1m"      /**< Bold/Bright text. */
#define ANSI_DIM "\x1b[2m"       /**< Dim/Faint text. */
#define ANSI_UNDERLINE "\x1b[4m" /**< Underlined text. */
#define ANSI_REVERSE "\x1b[7m"   /**< Swaps foreground and background colors. */
#define ANSI_BLINK "\x1b[5m"     /**< Blinking text (support varies by terminal). */
///@}

/**
 * @name Standard Foreground Colors
 * @brief Basic ANSI colors.
 */
///@{
#define ANSI_COLOR_BLACK "\x1b[30m"
#define ANSI_COLOR_RED "\x1b[31m"    /**< Error messages. */
#define ANSI_COLOR_GREEN "\x1b[32m"  /**< Success messages. */
#define ANSI_COLOR_YELLOW "\x1b[33m" /**< Warnings or highlights. */
#define ANSI_COLOR_BLUE "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN "\x1b[36m"
#define ANSI_COLOR_WHITE "\x1b[37m"
///@}

/**
 * @name Bright Foreground Colors
 * @brief High-intensity versions of standard colors.
 */
///@{
#define ANSI_BRIGHT_BLACK "\x1b[90m" /**< Dark Gray (often used for comments/hints). */
#define ANSI_BRIGHT_RED "\x1b[91m"
#define ANSI_BRIGHT_GREEN "\x1b[92m"
#define ANSI_BRIGHT_YELLOW "\x1b[93m"
#define ANSI_BRIGHT_BLUE "\x1b[94m"
#define ANSI_BRIGHT_MAGENTA "\x1b[95m"
#define ANSI_BRIGHT_CYAN "\x1b[96m"
#define ANSI_BRIGHT_WHITE "\x1b[97m"
///@}

/* --- Display Functions --- */

/**
 * @brief Clears the terminal screen and resets the cursor position.
 *
 * Uses standard ANSI escape sequences (`\033[H\033[J`) to wipe the visual buffer
 * and move the cursor to the top-left corner (0,0).
 * This provides a cleaner UI transition between queries or menus.
 */
void clear_screen(void);

/**
 * @brief Renders the Main Menu options to stdout.
 *
 * Displays the high-level choices available to the user in the REPL loop, such as:
 * - [1] Load Dataset
 * - [2] Execute Queries
 * - [3] View Files
 * - [5] Exit
 */
void print_options(void);

/**
 * @brief Renders the Query Selection Menu.
 *
 * Lists all available queries (Q1 through Q10) with brief descriptions of their
 * functionality, helping the user choose the correct command.
 */
void print_queries(void);

#endif // UI_H