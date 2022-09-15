class PkgSpec(object):
    def __init__(self, name, version, build, build_number):
        self.name = name
        self.version = version
        self.build = build
        self.build_number = build_number

    def __repr__(self):
        return f"{self.name}={self.version}={self.build}"

    def filename(self, extension="tar.bz2"):
        return f"{self.name}-{self.version}-{self.build}.{extension}"

    def format_url(self, url_template):
        return url_template.format(
            name=self.name, version=self.version, build=self.build
        )
