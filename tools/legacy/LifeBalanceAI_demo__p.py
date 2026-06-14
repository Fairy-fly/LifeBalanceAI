with open('mainwindow.ui','r',encoding='utf-8') as f: text=f.read()  
NL=chr(10)  
LT=chr(60)  
GT=chr(62)  
QT=chr(34)  
old='        '+LT+'item row='+QT+'5'+QT+' column='+QT+'0'+QT+GT+NL+'         '+LT+'widget class='+QT+'QPushButton'+QT+' name='+QT+'btnSaveProfile'+QT+GT  
new='        '+LT+'item row='+QT+'5'+QT+' column='+QT+'0'+QT+GT+NL+'         '+LT+'widget class='+QT+'QLineEdit'+QT+' name='+QT+'editAllergy'+QT+GT+NL+'          '+LT+'property name='+QT+'placeholderText'+QT+GT+NL+'           '+LT+'string'+GT+chr(0x8fc7)+chr(0x654f)+chr(0x4fe1)+chr(0x606f)+chr(0xff08)+chr(0x5982)+chr(0x65e0)+chr(0x8bf7)+chr(0x7559)+chr(0x7a7a)+chr(0xff09)+LT+'/string'+GT+NL+'          '+LT+'/property'+GT+NL+'         '+LT+'/widget'+GT+NL+'        '+LT+'/item'+GT+NL+'        '+LT+'item row='+QT+'6'+QT+' column='+QT+'0'+QT+GT+NL+'         '+LT+'widget class='+QT+'QPushButton'+QT+' name='+QT+'btnSaveProfile'+QT+GT  
text=text.replace(old, new)  
with open('mainwindow.ui','w',encoding='utf-8') as f: f.write(text)  
print('ui updated')  
