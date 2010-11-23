#ifndef WTF_CONFIG_H
#define WTF_CONFIG_H

/*
 * Define wtfDND_FULL to gain full Drag'n'Drop functionality
 * This means, that regardless whether wtfDND_FULL defined or not,
 * the utility supports addition of files via D'n'D from outside.
 * You may define wtfDND_FULL to gain possibility of dragging
 * files from utility's windows wherever you wish.
 * Note: your wxWidgets port should have this ability, otherwise
 * you'll get compilation errors (with wtfDND_FULL defined).
 * Currently only wxMSW (2.8.9) do support this feature
 */
#define wtfDND_FULL

/*
 * Debugging features
 *
 * Here is the list of available defines and explanations
 *
 *   wtfDEBUG_DISAMBIGUATE - log messages during work of function Disambiguate
 *   wtfDEBUG_GENC - log messages during work of function wtfGEncoding::Guess
 *   wtfDEBUG_GENC_CALC - log additional messages on calculation process
 *      during work of function wtfGEncoding::Guess
 *   wtfDEBUG_MEMORY_LEAKS - log messages on instances creation/deletion
 *      Note: __WXDEBUG__ must be defined
 *   wtfDEBUG_PROCESS_SINGLE - log messages during work of function
 *      wtfTorrentTab::ProcessSingle
 *   wtfDEBUG_PROCESS_MULTI - log messages during work of function
 *      wtfTorrentTab::ProcessMulti
 *
 *
 */

//#define wtfDEBUG_DISAMBIGUATE
//#define wtfDEBUG_PROCESS_MULTI
//#define wtfDEBUG_MEMORY_LEAKS

#endif // WTF_CONFIG_H

