# CI でのビルド

## githash.bat で設定する環境変数

|||ローカルビルド|Appveyor|Azure Pipelines|GitHub Actions|gitbash.h への出力|
|--|--|--|--|--|--|
|GIT_ORIGIN_URL|git remote origin URL|◎|◎|◎|◎|◎|
|GIT_LONG_HASH|git の commit Hash|◎|◎|◎|◎|◎|
|GIT_ABBR_HASH|git の commit Hash 短縮形|◎|◎|◎|◎|◎|
|GIT_TAG_NAME|git の tag|◎|◎|◎|◎|◎|
|CI_SOURCE_REPO|リポジトリ名|×|◎|◎|◎|×|
|CI_SOURCE_URL|GitHub の Commit URL|×|◎|◎|◎|◎|
|CI_BUILD_VERSION|CI のビルドバージョン (文字列)|×|◎|◎|◎|◎|
|CI_BUILD_URL|CI のビルドURL|×|◎|◎|◎|◎|
|PR_NUMBER|GitHub の PR 番号|×|○ (PRのみ)|○ (PRのみ)|○ (PRのみ)|×|
|PR_NAME|GitHub の PR 番号|×|○ (PRのみ)|○ (PRのみ)|○ (PRのみ)|○ (PRのみ)|
|PR_SOURCE_URL|GitHub の PR の Head の URL|×|○ (PRのみ)|○ (PRのみ)|○ (PRのみ)|○ (PRのみ)|
|DEV_VERSION|開発バージョンかどうか|◎|◎|◎|◎|◎|

## 入力として使用する環境変数

### Appveyor

| 環境変数 | 説明 |
----|---- 
|APPVEYOR                           | appveyor で実行されている場合`"True"`  |
|APPVEYOR_ACCOUNT_NAME              | appveyor のアカウント名 (sakura editor の場合 "sakuraeditor") |
|APPVEYOR_BUILD_NUMBER              | ビルド番号 |
|APPVEYOR_URL                       | https://ci.appveyor.com |
|APPVEYOR_BUILD_VERSION             | appveyor.yml の version フィールドの値 |
|APPVEYOR_BUILD_ID                  | ビルドID (ビルド結果URLに含まれる数値です。`build-chm.bat`が実行中のビルドを識別するために使います。) |
|APPVEYOR_PROJECT_SLUG              | project slug (appveyor の URL 名) |
|APPVEYOR_PULL_REQUEST_NUMBER       | Pull Request 番号 |
|APPVEYOR_PULL_REQUEST_HEAD_COMMIT  | Pull Request の Head commit Hash |
|APPVEYOR_REPO_NAME                 | リポジトリ名 (owner-name/repo-name) |
|APPVEYOR_REPO_PROVIDER             | appveyor の参照するリポジトリ種別 (GitHub の場合 "gitHub") |
|~~READONLY_TOKEN~~                 | デバッグ用です。 appveyor の REST API に渡す [Bearer Token](https://www.appveyor.com/docs/api/#Authentication) をスクリプト外から渡せるように定義しています。 appveyor では使いません。(未定義なので値は''になります。) |

APPVEYOR_REPO_TAG_NAME は利用をやめて 代わりに GIT_TAG_NAME を使うようにしました。[#876](https://github.com/sakura-editor/sakura/pull/876)

* 上記環境変数をローカル環境で set コマンドで設定することにより appveyor でビルドしなくてもローカルでテストできます。
* 上記の環境変数がどんな値になるのかは、過去の appveyor ビルドでのログを見ることによって確認できます。
* `build-chm.bat`をローカルでテストするには完了済みのビルドIDが必要です。ビルドIDは[history](https://ci.appveyor.com/project/sakuraeditor/sakura/history)から各ビルド結果を表示するとURL末尾に付いている数字です。

### Azure Pipelines

|環境変数|説明|
|--|--|
|TF_BUILD|Azure Pipelines で実行されている場合`"True"`|
|BUILD_REPOSITORY_NAME|リポジトリ名 (owner-name/repo-name)|
|BUILD_DEFINITIONNAME|アカウント名|
|BUILD_BUILDID|ビルド番号 (数値)|
|BUILD_BUILDNUMBER|ビルドバージョン (文字列)|
|SYSTEM_PULLREQUEST_PULLREQUESTNUMBER|Pull Request 番号|
|SYSTEM_PULLREQUEST_SOURCECOMMITID|Pull Request の Head commit Hash|
|BUILD_REPOSITORY_PROVIDER|Azure Pipelines の参照するリポジトリ種別 (GitHub の場合 "GitHub") |

## githash.h で生成するマクロ

|生成するマクロ名|元にする環境変数|型|
|--|--|--|
|GIT_ORIGIN_URL|GIT_ORIGIN_URL|文字列|
|GIT_LONG_HASH|GIT_LONG_HASH|文字列|
|GIT_ABBR_HASH|GIT_ABBR_HASH|文字列|
|GIT_TAG_NAME|GIT_TAG_NAME|文字列|
|DEV_VERSION|-|-|
|CI_SOURCE_URL|CI_SOURCE_URL|文字列|
|CI_BUILD_VERSION|CI_BUILD_VERSION|文字列|
|CI_BUILD_URL|CI_BUILD_URL|文字列|
|PR_NAME|PR_NUMBER|文字列|
|PR_SOURCE_URL|PR_SOURCE_URL|文字列|

## zipArtifacts.bat で設定する環境変数

### 生成する環境変数

| 生成する環境変数 | 説明 | 有効性 |
----|----|----
| ALPHA         | alpha バージョンの場合 1                           | x64 ビルドの場合                       |
| BUILD_ACCOUNT | CI のビルドアカウント名                            | sakura editor 用のアカウントの場合空   |
| TAG_NAME      | "tag_" + tag 名                                    | tag が有効な場合                       |
| BUILD_NUMBER  | "build" + ビルド番号                               | CI ビルド以外の場合 "buildLocal"       |
| PR_NAME       | "PR" + PR番号                                      | CI での PR のビルドのみ有効            |
| SHORTHASH     | commit hash の先頭8文字                            | 実体は GIT_ABBR_HASH      |
| RELEASE_PHASE | "alpha" または 空                                  | x64 ビルドの場合のみ有効               |
| BASENAME      | 成果物の zip ファイル名(拡張子含まない部分)        | 常に有効                               |
| WORKDIR       | 作業用フォルダ                                     | 常に有効                               |
| WORKDIR_LOG   | ログファイル用の作業用フォルダ                     | 常に有効                               |
| WORKDIR_EXE   | 実行ファイル(一般向け)用の作業用フォルダ           | 常に有効                               |
| WORKDIR_DEV   | 開発者向け成果物用の作業用フォルダ                 | 常に有効                               |
| WORKDIR_INST  | インストーラ用の作業用フォルダ                     | 常に有効                               |
| WORKDIR_ASM   | アセンブラ出力用の作業用フォルダ                   | 常に有効                               |
| OUTFILE       | 成果物の zip ファイル名                            | 常に有効                               |
| OUTFILE_LOG   | ログファイルの成果物の zip ファイル名              | 常に有効                               |
| OUTFILE_EXE   | 実行ファイル(一般向け)の成果物の zip ファイル名    | 常に有効                               |
| OUTFILE_DEV   | 開発者向け成果物の zip ファイル名                  | 常に有効                               |
| OUTFILE_INST  | インストーラの成果物の zip ファイル名              | 常に有効                               |
| OUTFILE_ASM   | アセンブラ出力の成果物の zip ファイル名            | 常に有効                               |
| HASHFILE      | sha256 のハッシュ値のファイル名                    | 常に有効                               |
