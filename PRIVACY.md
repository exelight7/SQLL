# Privacy of the published files

This publication excludes machine-specific baseline reports, local configuration, runtime logs, crash dumps, private keys, credentials, and local IDE settings. Compiler prefix mapping keeps the build machine's checkout/toolchain paths out of the DLL's diagnostic strings.

The release verification contains public file names, checksums, and test summaries. Git commits use the maintainer's GitHub noreply identity. Copyright notices and intentionally public account/repository names are retained.

`privacy_scan.py` checks the tracked source and release assets for local user paths, the current build account name, credential patterns, unexpected literal IPv4 addresses, and private-key files. Recognized dotted software versions are classified separately. Font/image data are kept as their original public assets. This is a repeatable screening process, not a guarantee that every possible sensitive value can be detected.

Before reporting a problem, remove player names/identifiers, addresses, session data, local paths, and credentials from logs. Do not upload a complete game crash dump or a personal settings folder.

The privacy scope here is the files published on GitHub. The MP3 gameplay behavior remains unchanged.
