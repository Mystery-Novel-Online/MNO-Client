#pragma once

class QString;

class VersionNumber;

int get_release_version();
int get_major_version();
int get_minor_version();
VersionNumber get_version_number();
QString get_post_version();
QString get_version_string();
QString get_about_message();

