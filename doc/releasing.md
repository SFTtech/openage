# Releasing

openage uses [Semver](https://semver.org).. People often seem to overlook the special rules for initial development versions. Please avoid prerelease versions (`-alpha`, `-beta`, `-rc.1`, ...) for now. We might decide to include build metadata like date or commit hash, that's up for discussion.

Release guide:

1. Create a new changelog named `doc/changelogs/engine/$VERSION.md`.
1. Bump the version number in `openage_version`. Given `0.y.z`, both `0.y.(z+1)` and `0.(y+1).0` are valid. The latter will probably be more common, but which of those you choose doesn't really matter.
1. Commit, PR, merge, pull as usual.
    * OPTIONAL: Set up [commit signing](https://help.github.com/en/articles/managing-commit-signature-verification), signing tags will make a "verified" badge show up on the release later.
1. Tag the merge commit, something like `git tag -s v0.5.0`. The prefixed `v` is mandatory.
1. Push the tag to GitHub.
1. Use the GitHub web interface to publish a release from the tag. Make it look nice, see previous release description. If in doubt, save the draft and have someone else proof read it. Also include the changelog.
1. In the future, kevin will automatically attach build artifacts like installers to the release. For now, you will need to do that manually. Have fun setting up a windows build environment or if you feel like being lazy, go bug someone who already has. Actually, taking care of where to build for windows before publishing the release might not be a bad idea.
    * OPTIONAL: Brag on social media. /r/openage, /r/aoe2 and AoE2 Discords might be the right place to do so.
