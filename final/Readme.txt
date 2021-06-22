(1) 接線

馬達(D5、D6)、OpenMW(D1、D0)、Xbee(D10、D9)、Ping(D13)

(2) 程式

OpenMV執行OpenMV.txt、板子執行main.cpp

(3) 車子

	(a) 車子沿線走 : 線為紅色線，弧度不可超過約45度(與電池電量有關)

	(b) 繞障礙物   : 障礙物必須在線終端，且可被ping照到，繞法為先原地轉90度，走個弧形直到照到AprilTag

	(c) AprilTag定位 : 繞過障礙物後必須可照到AprilTag，車子會往AprilTag走，並利用ping測距，當距離到達70就停止

	(d) 車子沿線走  : 車子停止後必須可照到紅色的線，走到線終點後停止。