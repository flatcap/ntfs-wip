@echo off

d:
cd\
rd /q/s p > nul
md p
cd p

for %%f in (0000 0100 0200 0300 0400 0500 0600) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

echo file9990 > file9990

for /l %%i in (0,1,4) do @(
	for /l %%j in (0,1,9) do @(
		@echo dummy > file9991_%%i%%j
	)
)

for %%f in (9992 9993 9994 9995 9996 9997 9998) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (0600 0700 0800 0900 1000 1100 1200 1300 1400 1500) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (1500 1600 1700 1800 1900 2000 2100 2200 2300 2400) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (2500 2600 2700) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (2700 2800 2900) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (2800 2900 3000) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (3000 3100 3200) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (3200 3300 3400) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (3400 3500 3600) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (3600 3700 3800) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (3800 3900 4000) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (4000 4100 4200) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (4200 4300 4400) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (4400 4500 4600) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (4600 4700 4800) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (4800 4900 5000) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (5000 5100 5200) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (5200 5300 5400) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (5400 5500 5600) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (5600 5700 5800) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for %%f in (5800 5900 6000) do @(

	echo file%%f > file%%f

	for /l %%i in (0,1,4) do @(
		for /l %%j in (0,1,9) do @(
			@echo dummy > file%%f_%%i%%j
		)
	)
)

del file????_??

for /l %%i in (6000,1,6118) do @(
	echo file%%i > file%%i
)

for %%f in (00 02 04 06 08 10 12 14 16 18 20 22 24 26 28 30 32 34 36 38 40 42 44 46 48 50 52 54 56 58) do @(

	for /l %%i in (10,1,46) do @(
		@echo file%%f%%i > file%%f%%i
	)
)

for /l %%i in (6000,20,6100) do @(
	for %%j in (a b c d e f g h i j k l m n o p q r s) do @(
		@echo file%%i%%j > file%%i%%j
	)
)

del file9997
del file9996
del file9995
del file9994

sync d > nul
