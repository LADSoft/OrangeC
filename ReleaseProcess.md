# Prerequisites for release



* passes Coverity scan
* builds locally
* builds in git actions
* builds on appveyor
* builds gnucobol using autoconf





# Release Process



* cull release notes from git commit log
* add release notes to the releasenotes file and the history file
* add the release date to the release notes
* update the version in orange/src/version.h
* update the version stem in orange/appveyory.yml
* go to the appveyor site and set the build number back to 1
* tag the release (vx.y.z)
* push the tag
* wait for the MSVC THEN WITH STATIC LIB build to complete
* Now there will be a release tag with artifacts in GitHub
* make an overview of the pertinent features in this release and add the line by line release notes
* add it to the release tag as documentation
* Change the release tag name to something more relevant
* update README.MD if necessary
* announce as necessary

