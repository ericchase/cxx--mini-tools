@rmdir test /s /q

@mkd test

@start "" .\watch.exe test1

@timeout 1

@mkd test/😊
@mkd test/test/😊
@mkd test/test/test/😊
@mkd test/test/test/test/😊
@mkd test/test/test/test/test/😊
@mkd test/test/test/test/test/test/😊
@mkd test/test/test/test/test/test/test/😊
