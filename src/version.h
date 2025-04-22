#pragma once

class QString;

class VersionNumber;

/// @returns The release version portion of the client version number.
int get_release_version();

/// @returns The major version portion of the client version number.
int get_major_version();

/// @returns The minor version portion of the client version number.
int get_minor_version();

/// @returns A `VersionNumber` object containing the client version number.
VersionNumber get_version_number();

/// @returns The post version of the client.
QString get_post_version();

/// @returns A string containing version information regarding the client.
QString get_version_string();

/// Builds an about message with the version of the client and some git details regarding the branch and commit hash.
///
/// @returns A string containing the about message displayed in settings.
QString build_about_message();

