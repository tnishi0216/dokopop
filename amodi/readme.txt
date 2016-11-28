amodiをいろいろと試してもcompile errorから逃れられないので、作り直した。
VS2013用

■注意事項
・対象の.NET frameworkのversionを一度でもV4.5未満に下げてしまうと、new MODIでruntimeエラーが発生してしまう。
  原因不明。
  V4.51への変更は問題なし。(V4.5へ戻しても問題ない)

■Tips
・MODIのプロジェクトへの追加方法
  VS2013の場合：
    - ソリューションエクスプローラーに「参照設定」の項目がある
    - そこを選択→参照の追加
    - 左側のペインより、「COM - タイプライブラリ」を選択
    - 一覧の中に「Microsoft Office Document Imaging 12.0 Type Libray」があるのでそれを選択してOK

・(MODI.Image)md.Images[i] のところでcompile errorが出るとき
  - 根本的な解決ではないが、amodi.csproj の中の次の項目を変更・削除する
    - "x86"をすべて"AnyCPU"に
    - TargetFrameworkVersionをv4.0からv4.5に変更
    - <Prefer32Bit>false</Prefer32Bit>を削除
    - <AllowUnsafeBlocks>true</AllowUnsafeBlocks>を削除
    これでcompile errorはthroughするようだ

