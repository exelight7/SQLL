from pathlib import Path

def privacy_flags(project: Path, toolchain: Path) -> list[str]:
    """Keep checkout/toolchain locations out of compiled macro and debug strings."""
    result = []
    for path, replacement in [(toolchain.resolve(), '<toolchain>'), (project.resolve(), '.')]:
        for spelling in sorted({str(path), path.as_posix()}):
            result += ['-ffile-prefix-map=' + spelling + '=' + replacement,
                       '-fdebug-prefix-map=' + spelling + '=' + replacement]
    return result
