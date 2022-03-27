# Info

[repo link]()



# Tutorial and Docs

[About `.gitignore`](https://git-scm.com/docs/gitignore)

[github docs](https://git-scm.com/docs/gitignore)

[github cli manual](https://cli.github.com/manual/)





# Intro and Start

## Starter

### md reader

强烈推荐使用markdown格式的文件作为说明文档、笔记文档的载体。

我个人使用的是[typora](https://typora.io)作为markdown阅读器和写作器，但是typora的最新版本是要付费使用的，你可以自己选择markdown阅读器（据说vs code是一个不错的选择）。

这里提供一个“协议”：所有markdown的图片全部放在和`.md`文件同文件夹下的图片文件夹中，并且在`.md`文件内使用相对路径引用图片。



### git and github

我们使用git来做多人协同，github作为公共仓库。

0. 不出意外的话，你需要科学上网，并且可能需要知道如何在terminal开启代理。

    在terminal执行以下代码来获得一些我们需要的应用：

    ```sh
    curl -fsSL https://cli.github.com/packages/githubcli-archive-keyring.gpg | sudo dd of=/usr/share/keyrings/githubcli-archive-keyring.gpg
    echo "deb [arch=$(dpkg --print-architecture) signed-by=/usr/share/keyrings/githubcli-archive-keyring.gpg] https://cli.github.com/packages stable main" | sudo tee /etc/apt/sources.list.d/github-cli.list > /dev/null
    sudo apt update
    sudo apt install gh
    sudo apt-get update
    sudo apt-get install git
    ```

    正常来说在执行完之后，在命令行输入`gh`和`git`都会显示相应的帮助信息。

1. 如果没有github账号的话，先登陆[github网站](https://github.com)注册github账号。

2. 之后根据[教程](https://medium.com/@jonathanmines/the-ultimate-github-collaboration-guide-df816e98fb67)成为我们仓库的collaborators。

3. 根据[教程](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/generating-a-new-ssh-key-and-adding-it-to-the-ssh-agent)在本地生成`ssh key`。

4. 在terminal输入`gh auto login`来关联git和github。下图是你可以参考的一些选项：

    ```sh
    ? What account do you want to log into? GitHub.com
    ? What is your preferred protocol for Git operations? HTTPS
    ? Authenticate Git with your GitHub credentials? Yes
    ? How would you like to authenticate GitHub CLI? Login with a web browser
    ```

    使用这些选项关联可以省略一些麻烦的事情（比如在github生成token）。

5. 根据[教程](https://docs.github.com/en/authentication/connecting-to-github-with-ssh/adding-a-new-ssh-key-to-your-github-account)把刚刚生成的`ssh-key`（注意`key-file`指的是`.pub`文件）提交给github。

    我在执行这一步之前需要先执行`gh auth refresh -h github.com -s admin:public_key`，并且执行`gh ssh-key add <key-file>`两次，我不清楚别的账号上是什么行为。。。

    在执行完后，进入[github-settings-keys](https://github.com/settings/keys)查看刚加入的key。

6. 

