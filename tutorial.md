# Tutorial to remember stuff

## Control File

A control file in PostgreSQL is a key component of an extension. It provides essential metadata about the extension to the PostgreSQL server. The control file is named after the extension with the suffix `.control` and must be placed in the PostgreSQL extension directory.

### Purpose of Control File:

The control file is used to:

- Identify the Extension: It provides the name of the extension and other key details.
- Manage Versions: It specifies the version of the extension and supports versioning for extension updates.
- Dependency Management: It can specify dependencies on other extensions or PostgreSQL modules.
- Configuration: It includes configuration parameters like the default superuser privileges for the extension.

### Contents of Control File:

Typically, a control file includes:

- Extension Name: The name of the extension.
- Default Version: The current version of the extension.
- Comment: A brief description of what the extension does.
- Schema: Optional. Specifies the default schema for the objects created by the extension.
- Dependencies: Optional. Lists other extensions that this extension depends on.
- Module Pathname: The name of the shared library (without the platform-specific prefix and suffix) for the extension.
- Requisite Files: The SQL script files needed to install the extension (and optionally to update it from previous versions).

### Example

Let's consider an example control file called `myextension.control` for a fictional extension named myextension:

    comment = 'My custom PostgreSQL extension'
    default_version = '1.0'
    module_pathname = '$libdir/myextension'
    relocatable = false
    schema = 'public'
    requires = 'other_extension'

In this example:

- comment: Describes the extension.
- default_version: Specifies the version of the extension, which should match a corresponding SQL file (e.g., myextension--1.0.sql for installation scripts).
- module_pathname: The location of the compiled binary code for the extension. $libdir is a placeholder for PostgreSQL's library directory.
- relocatable: Indicates whether the extension can be installed in a schema different from the one specified. false means it's not relocatable.
- schema: The default schema where the extension will be installed.
- requires: Specifies any other extensions that are needed for this extension to work.

In summary, the control file is vital for defining the properties and dependencies of a PostgreSQL extension, and it informs PostgreSQL about how to handle the extension during installation and updates.