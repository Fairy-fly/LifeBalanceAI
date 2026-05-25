$file = "D:\QtProjects\LifeBalanceAI\LifeBalanceAI_demo\animateddialog.cpp"
$content = [System.IO.File]::ReadAllText($file, [System.Text.Encoding]::UTF8)

# Line 54: garbled ȷ�� -> proper \u786e\u5b9a (确定)
$old1 = "QString::fromUtf8(`"ȷ��`"), this);`r`n    m_ok->setFixedSize(80, 30);"
$new1 = "QString::fromUtf8(`"\u786e\u5b9a`"), this);`r`n    m_ok->setFixedSize(80, 30);"
$content = $content.Replace($old1, $new1)

# Line 322: garbled ȡ�� -> proper \u53d6\u6d88 (取消)
$old2 = "QString::fromUtf8(`"ȡ��`"), this);`r`n    m_ccl->setFixedSize(72,32);"
$new2 = "QString::fromUtf8(`"\u53d6\u6d88`"), this);`r`n    m_ccl->setFixedSize(72,32);"
$content = $content.Replace($old2, $new2)

# Line 328: garbled ȷ�� -> proper \u786e\u5b9a (确定) second occurrence
$old3 = "QString::fromUtf8(`"ȷ��`"), this);`r`n    m_ok->setFixedSize(72,32);"
$new3 = "QString::fromUtf8(`"\u786e\u5b9a`"), this);`r`n    m_ok->setFixedSize(72,32);"
$content = $content.Replace($old3, $new3)

[System.IO.File]::WriteAllText($file, $content, [System.Text.Encoding]::UTF8)
Write-Output "animateddialog.cpp fixed (3 replacements)"
