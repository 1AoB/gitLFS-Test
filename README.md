# 一、gitLFS-Test
测试-Git LFS (Large File Storage)

# 二、gitLFS的作用

### 1️⃣ 不用 Git LFS（普通 Git）

```
Git 仓库 = 源码 + 所有历史版本的二进制
```

- 每次 `.lib` 改动：
  - Git 永久保存一个新 blob（**blob = Git 用来存“文件内容”的最小存储单元**  ， 👉 只存内容，不存文件名、不存路径、不存权限）
- 即使你 reset / revert / 删除文件：
  - 历史里的 blob 仍然存在
- 仓库会越来越大（不可逆）

------

### 2️⃣ 用 Git LFS

```
Git 仓库 = 源码 + LFS 指针
LFS Server = 二进制内容 + 历史版本
```

- Git 仓库里：
  - **只有几十字节的 pointer**
- LFS server：
  - 保存 **每一次提交对应的二进制版本**
- checkout 旧 commit：
  - LFS 会拉 **那个 commit 对应的旧版本二进制**

👉 **回退是完全没问题的**

------

### 3️⃣ 真实回退流程

假设你有三次提交：

```
A --- B --- C (HEAD)
```

每次 `.lib` 都不一样。

```
git checkout A
```

发生的事是：

1. Git checkout 到 commit A
2. Git 发现该 commit 指向某个 LFS oid
3. Git LFS 自动去 server 拉 **A 对应的 .lib**
4. 本地拿到的是 **A 版本的二进制**



即：
✔️ 完全可回退
✔️ 不丢历史
✔️ 不影响源码

------

# 三、那“Git 仓库不保存大二进制”到底是什么意思？

这是一个**存储层面的概念**，不是**版本语义层面的**。

### ❌ 不是说

- 不保存历史
- 不能回退
- 只留最新文件

### ✅ 而是说

- `.git/objects` 里 **不再存二进制内容**
- Git 的 packfile 不会被 `.lib` 撑爆
- clone / fetch 速度不会随历史暴涨

------

# 四、那如果我「上传了一个不该上传的二进制」怎么办？

你这个问题问得**非常到位**，我们分两种情况。

------

### 情况 1：只是“内容不对”，但不敏感、不违规

✅ **完全没问题**

- 你可以：

  ```
  git revert
  ```

  或

  ```
  git reset
  ```

- Git LFS 会：

  - 拉回旧版本的二进制
  - 仍然可编译 / 可运行

------



------

# 五、那 Git LFS 的“真正收益”是什么？

结合你是 **C++ / Qt / .lib** 这种背景，说白了就三点：

### ✅ 1. Git 仓库不会被编译产物拖死

- `.lib / .dll / .exe`
- 即使改 100 次
- `.git` 体积仍然可控

------

### ✅ 2. 新人 clone 项目不会被历史文件坑死

- 不用拉 2 年前的 `.lib`
- 只拉当前 commit 需要的版本

------

### ✅ 3. Git 操作仍然是“快的”

- `git status`
- `git diff`
- `git gc`

不会被大文件拖慢

### ✅ 补充一点，有人可能不知道：

不用 LFS 时，git clone 会把「历史里的旧版本二进制」也一起拉下来。

那从另一个角度来讲，不使用lfs，如果git clone之后，如果想切换到另一个版本，无需重新联网再次拉去，而使用了lfs则需要重新联网重服务器上找对应的版本

## 结论（精确版）

> ✅ **不用 LFS：一次 clone，之后切换任意历史版本都可以完全离线**
>  ⚠️ **用 LFS：切换到“本地没缓存过的版本”，需要联网从 LFS 服务器拉对应对象**

------


# 六、如何使用？

```bash
准备好lib，然后一起上传

（注意，lfs不会递归，你必须指定到lib的当前目录为止，如果你只是弄一个lib*,那是没有效果的）
git lfs track lib/Debug/*
git lfs track lib/Release/*

git add .

查看是否成功：(不用提交了再看，add之后就可以查看是否成功了)
git lfs ls-files 

git commit -m 'add .gitattributes'

git push

  




```

eg：

```
wxn@LAPTOP-0EEC12D6 MINGW64 /d/tmp/gitLFS-Test (main)
$ git lfs track lib/Debug/*  lib/Release/*
Tracking "lib/Debug/addlib.lib"
Tracking "lib/Release/addlib.lib"

wxn@LAPTOP-0EEC12D6 MINGW64 /d/tmp/gitLFS-Test (main)
$ git add .

wxn@LAPTOP-0EEC12D6 MINGW64 /d/tmp/gitLFS-Test (main)
$ git commit -m 'add .gitattributes'
[main a66391a] add .gitattributes
 3 files changed, 8 insertions(+)
 create mode 100644 .gitattributes
 create mode 100644 lib/Debug/addlib.lib
 create mode 100644 lib/Release/addlib.lib

wxn@LAPTOP-0EEC12D6 MINGW64 /d/tmp/gitLFS-Test (main)
$ git lfs ls-files
d55c149395 * lib/Debug/addlib.lib
c6953e95fa * lib/Release/addlib.lib

wxn@LAPTOP-0EEC12D6 MINGW64 /d/tmp/gitLFS-Test (main)
$ git push
Uploading LFS objects: 100% (2/2), 5.5 KB | 0 B/s, done.
Counting objects: 8, done.
Delta compression using up to 12 threads.
Compressing objects: 100% (6/6), done.
Writing objects: 100% (8/8), 770 bytes | 0 bytes/s, done.
Total 8 (delta 1), reused 0 (delta 0)
remote: Resolving deltas: 100% (1/1), completed with 1 local object.
To git@github.com:1AoB/gitLFS-Test.git
   41df34c..a66391a  main -> main


```

