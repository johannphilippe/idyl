#!/usr/bin/env python3
"""
Build a .vsix package for the idyl-lang VS Code extension.

Usage:
    python3 build_vsix.py [--install]

Options:
    --install   Install the .vsix into the local VS Code after building.

The version is read from idyl-lang/package.json.
The output file is placed next to this script: idyl-lang-<version>.vsix
"""

import sys
import os
import json
import zipfile
import subprocess

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
EXT_DIR    = os.path.join(SCRIPT_DIR, "idyl-lang")

# Files to include (relative to EXT_DIR)
INCLUDE = [
    "package.json",
    "extension.js",
    "language-configuration.json",
    "syntaxes/idyl.tmLanguage.json",
    "syntaxes/idyl-markdown-injection.json",
]


def read_version():
    pkg = os.path.join(EXT_DIR, "package.json")
    with open(pkg) as f:
        return json.load(f)["version"]


def content_types_xml():
    return """\
<?xml version="1.0" encoding="utf-8"?>
<Types xmlns="http://schemas.openxmlformats.org/package/2006/content-types">
  <Default Extension=".json" ContentType="application/json" />
  <Default Extension=".vsixmanifest" ContentType="text/xml" />
</Types>"""


def vsix_manifest(pkg):
    return f"""\
<?xml version="1.0" encoding="utf-8"?>
<PackageManifest Version="2.0.0"
  xmlns="http://schemas.microsoft.com/developer/vsx-schema/2011"
  xmlns:d="http://schemas.microsoft.com/developer/vsx-schema-design/2011">
  <Metadata>
    <Description xml:space="preserve">{pkg["description"]}</Description>
    <DisplayName>{pkg["displayName"]}</DisplayName>
    <Identity Language="en-US" Id="{pkg["name"]}" Version="{pkg["version"]}" Publisher="{pkg["publisher"]}" />
    <Tags>idyl,language,syntax</Tags>
    <GalleryFlags>Public</GalleryFlags>
    <Properties>
      <Property Id="Microsoft.VisualStudio.Code.Engine" Value="{pkg["engines"]["vscode"]}" />
    </Properties>
  </Metadata>
  <Installation>
    <InstallationTarget Id="Microsoft.VisualStudio.Code"/>
  </Installation>
  <Dependencies/>
  <Assets>
    <Asset Type="Microsoft.VisualStudio.Code.Manifest" Path="extension/package.json" Addressable="true" />
  </Assets>
</PackageManifest>"""


def build(install=False):
    with open(os.path.join(EXT_DIR, "package.json")) as f:
        pkg = json.load(f)

    version = pkg["version"]
    out = os.path.join(SCRIPT_DIR, f"idyl-lang-{version}.vsix")

    missing = [p for p in INCLUDE if not os.path.exists(os.path.join(EXT_DIR, p))]
    if missing:
        print(f"Error: missing files: {missing}", file=sys.stderr)
        sys.exit(1)

    with zipfile.ZipFile(out, "w", zipfile.ZIP_DEFLATED) as z:
        z.writestr("[Content_Types].xml", content_types_xml())
        z.writestr("extension.vsixmanifest", vsix_manifest(pkg))
        for rel in INCLUDE:
            z.write(os.path.join(EXT_DIR, rel), f"extension/{rel}")
            print(f"  + extension/{rel}")

    size_kb = os.path.getsize(out) / 1024
    print(f"\nBuilt: {out}  ({size_kb:.1f} KB)")

    if install:
        print("Installing...")
        result = subprocess.run(["code", "--install-extension", out], capture_output=True, text=True)
        output = (result.stdout + result.stderr).strip()
        if output:
            print(output)
        if result.returncode != 0:
            print("Warning: install command returned non-zero exit code.", file=sys.stderr)
        else:
            print("Installed. Reload the VS Code window to activate.")


if __name__ == "__main__":
    install_flag = "--install" in sys.argv
    build(install=install_flag)
