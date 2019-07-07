dim unlock_cmd
unlock_cmd="manage-bde -unlock -password "

Dim drives
drives = Array("d","e","f")

Dim passwd
passwd = InputBox("Enter password:","Unlock My Hard Drives")

If IsEmpty(passwd) Then
    WScript.Quit
End If

For each d in drives
    Set WshShell = Wscript.CreateObject("Wscript.Shell")
    WshShell.SendKeys unlock_cmd
    WshShell.SendKeys d
    WshShell.SendKeys ":"
    WshShell.SendKeys "{ENTER}"
    WshShell.SendKeys passwd
    WshShell.SendKeys "{ENTER}"    
Next
