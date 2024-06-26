# How to update the vcpkg port

In this project:

+ Update `set(antlr4_testrig_runner_VERSION x.y.z)`, if applicable.
+ Run git commands:

```
git add .
git status
git commit -m "Some message"
git push origin main
git rev-parse HEAD
```

In vcpkg overlay repo:

In ports/<portname>/portfile.cmake:
+ Set REF in portfile.cmake to HEAD of project repo.
+ Set SHA512 in portfile.cmake to hash.  See following commands to get the hash:

```
wget https://github.com/<USER>/<PROJECT>/archive/<REF>.tar.gz
vcpkg hash <REF>.tar.gz
rm <REF>.tar.gz
```

+ In versions/baseline.json, find entry for <port-name> and update "baseline" to newest project version (if project version changed) and "port-version" (if port was changed without project version changing).  I believe you can reset port-version back to 0 if you're updating to a new project version in "baseline".  Note that port-version is a single integer; it should NOT be in quotes.
+ In ports/<port-name>/vcpkg.json, update "version" and "port-version", if applicable.
+ Add and commit changes but DO NOT push yet.  You need to find out the updated ref for the project's folder in ports after the commit:
```
git add .
git commit -m "some message"
git rev-parse HEAD:./ports/<port-name>
```

+ In versions/x-/<port-name>.json, update or create an entry for this version and port-version.  I guess as to whether you change the existing entry or add new depends on if you want to keep old versions available.  I'm unsure whether you can have multiple entries with the same version and different port-versions, or if you just have to update the port-version in the one entry.
+ In the "versions" entry, whether you are updating or creating new, set the "git-tree" value to the value you just got from git rev-parse, above.
+ Now you need to amend the commit (without making a new commit; without changing ANYTHING else) to include the change to "git-tree".  Unlike normal commits, if you forget to git add the changed file, git WILL NOT warn you that the (amend to the) commit is empty.  This step is so error-prone that I recommend only ever running this step with "git add . && " in front of it:
```
git add . && git commit --amend --no-edit
git rev-parse HEAD:./ports/<port-name>
git push origin main
git rev-parse HEAD
```

+ I'm paranoid and always re-run the command to double check that the ref of the port folder didn't change from what we put into "git-tree"
+ Finally we push the overlay registry repo to Github and also get the ref of current HEAD; we'll need this for the baseline in the user project.

In the project using the port, in its vcpkg-configuration.json, update the "baseline" for the overlay registry to the ref of current HEAD we just got.  In the user project vcpkg.json, update the "version>=" value if we are using that.