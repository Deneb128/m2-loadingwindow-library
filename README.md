# m2-loadingwindow-library

Simple library made to show the progress of the loading of the Metin2Client while it's trying to load some data to memory.

Start of the library:
```c++
	auto LoadingWindow = std::thread(LoadingWindow::InitializeLibrary); //lib start
	CPythonApplication *app = new CPythonApplication(); //main thread initialization
	app->Initialize(hInstance);
```

Usage example:
```c++
LoadingWindow::UpdateProgressText("Executing tasks...");
for (auto i = 0; i < EXECUTION_TIMES; ++i)
{
	progress += progress_adder;
	// call a function here
	LoadingWindow::UpdateProgress(progress);
}
```

Result: https://streamable.com/5dxby3

  
