function Component()
{
	// accept some error codes as valid return values
	// 3010 means a reboot is needed (however this isn't most of the time)
	// 5100 means a newer version is already installed
	component.addElevatedOperation("Execute", "{0,3010,5100}", "@TargetDir@\\vc_redist.x64.exe", "/quiet", "/norestart");
	component.addOperation("Delete", "@TargetDir@\\vc_redist.x64.exe");
}