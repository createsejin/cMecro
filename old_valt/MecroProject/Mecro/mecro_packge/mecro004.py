from time import sleep
import mss
import clipboard


class Arduino001:
    debug_pix = False

    def __init__(self, ser, delay):
        self.ser = ser
        self.delay = delay

    def transSerial(self, str1):
        self.ser.write(str1.encode())

    def keyPrint(self, str1):
        sub1 = 'KP'
        trans = sub1 + str1
        self.transSerial(trans)

    def keyPress(self, sub2):
        sub1 = 'KD'
        trans = sub1 + sub2
        self.transSerial(trans)

    def keyRelease(self, sub2):
        sub1 = 'KU'
        trans = sub1 + sub2
        self.transSerial(trans)

    def wearing_scarf(self, H_state):
        down = H_state - 1 + 3
        self.keyPress('m')
        sleep(0.072)
        self.keyRelease('m')
        sleep(self.delay)
        for i in range(0, down):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('AD')
        sleep(self.delay)
        self.keyRelease('AD')
        sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        for i in range(0, 4):
            self.keyPress('AU')
            sleep(self.delay)
            self.keyRelease('AU')
            sleep(self.delay)
        self.keyPress('AR')
        sleep(self.delay)
        self.keyRelease('AR')
        sleep(self.delay)
        self.keyPress('m')
        sleep(self.delay)
        self.keyRelease('m')
        sleep(self.delay)
        print("-> wearing a scarf")
        return False

    def air_helmet(self, H_state):
        down = H_state - 1 + 3
        self.keyPress('m')
        sleep(0.072)
        self.keyRelease('m')
        sleep(self.delay)
        for i in range(0, down):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        for i in range(0, 8):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        print("-> air helmet check")
        return False

    def get_sticky_bomb(self, H_state):
        down = H_state - 1 + 2
        self.keyPress('m')
        sleep(0.072)
        self.keyRelease('m')
        sleep(self.delay)
        for i in range(0, down):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        for i in range(0, 5):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('AD')
        sleep(self.delay)
        self.keyRelease('AD')
        sleep(self.delay)
        for i in range(0, 1):
            self.keyPress('AR')
            sleep(self.delay)
            self.keyRelease('AR')
            sleep(self.delay)
        for i in range(0, 2):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('m')
        sleep(self.delay)
        self.keyRelease('m')
        sleep(self.delay)
        print("-> buy sticky bombs.")
        return False

    def call_SchafterLWB(self, H_state, motorOrCEO):
        if motorOrCEO != 2 or H_state < 2:
            print("CEO가 아니므로 샤프터를 소환할 수 없습니다.")
            return False
        elif motorOrCEO == 2 and H_state > 1:
            self.keyPress('m')
            sleep(0.072)
            self.keyRelease('m')
            sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            for i in range(0, 2):
                self.keyPress('AU')
                sleep(self.delay)
                self.keyRelease('AU')
                sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            for i in range(0, 10):
                self.keyPress('AU')
                sleep(self.delay)
                self.keyRelease('AU')
                sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            print("-> call CEO Schafter LWB")
            return False
        else:
            print("CEO가 아니므로 산체스를 소환할 수 없습니다.")
            return False

    def call_Sanchez(self, H_state, motorOrCEO):
        if motorOrCEO != 2 or H_state < 2:
            print("CEO가 아니므로 산체스를 소환할 수 없습니다.")
            return False
        elif motorOrCEO == 2 and H_state > 1:
            self.keyPress('m')
            sleep(0.072)
            self.keyRelease('m')
            sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            for i in range(0, 2):
                self.keyPress('AU')
                sleep(self.delay)
                self.keyRelease('AU')
                sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            for i in range(0, 4):
                self.keyPress('AU')
                sleep(self.delay)
                self.keyRelease('AU')
                sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            print("-> call CEO Sanchez")
            return False
        else:
            print("CEO가 아니므로 산체스를 소환할 수 없습니다.")
            return False

    def relocation_Kosatka(self, H_state):
        down = H_state - 1 + 5
        self.keyPress('m')
        sleep(0.072)
        self.keyRelease('m')
        sleep(self.delay)
        for i in range(0, down):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('AU')
        sleep(self.delay)
        self.keyRelease('AU')
        sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        for i in range(0, 3):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('AD')
        sleep(self.delay)
        self.keyRelease('AD')
        sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        print("-> relocation Kosatka")
        return False

    def ready_teleport(self):
        delay = 0.032
        self.keyPress('RC')
        sleep(delay)
        self.keyRelease('RC')
        sleep(0.65)
        self.keyPress('AL')
        sleep(delay)
        self.keyRelease('AL')
        sleep(0.05)
        self.keyPress('ET')
        sleep(delay)
        self.keyRelease('ET')
        sleep(0.07)
        self.keyPress('AU')
        sleep(delay)
        self.keyRelease('AU')
        sleep(0.07)
        self.keyPress('ET')
        sleep(delay)
        self.keyRelease('ET')
        sleep(0.05)
        self.keyPress('ET')
        sleep(delay)
        self.keyRelease('ET')
        sleep(0.05)
        self.keyPress('ET')
        sleep(delay)
        self.keyRelease('ET')
        sleep(0.7)
        self.keyPress('RC')
        sleep(delay)
        self.keyRelease('RC')
        sleep(0.65)
        self.keyPress('AL')
        sleep(delay)
        self.keyRelease('AL')
        sleep(0.15)
        self.keyPress('ET')
        sleep(delay)
        self.keyRelease('ET')
        sleep(0.3)
        self.keyPress('BS')
        sleep(delay)
        self.keyRelease('BS')
        sleep(0.15)
        self.keyPress('BS')
        sleep(delay)
        self.keyRelease('BS')
        print("-> teleport is ready.")
        return False

    def call_speedo_custom(self, H_state):
        down = H_state - 1 + 4
        self.keyPress('m')
        sleep(0.072)
        self.keyRelease('m')
        sleep(self.delay)
        for i in range(0, down):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        for i in range(0, 2):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('m')
        sleep(self.delay)
        self.keyRelease('m')
        sleep(self.delay)
        print("-> call Speedo Custom")
        return False

    def call_Phantom_Wedge(self, H_state):
        down = H_state - 1 + 4
        self.keyPress('m')
        sleep(0.072)
        self.keyRelease('m')
        sleep(self.delay)
        for i in range(0, down):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        for i in range(0, 2):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        for i in range(0, 3):
            self.keyPress('AR')
            sleep(self.delay)
            self.keyRelease('AR')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('m')
        sleep(self.delay)
        self.keyRelease('m')
        sleep(self.delay)
        print("-> call Phantom Wedge")
        return False

    def call_dinghy(self, H_state):
        self.keyPress('m')
        sleep(0.072)
        self.keyRelease('m')
        sleep(self.delay)
        down = H_state - 1 + 5
        for i in range(0, down):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('AU')
        sleep(self.delay)
        self.keyRelease('AU')
        sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        for i in range(0, 2):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        print("-> call Dinghy")
        return False

    def save_dinghy(self, H_state):
        self.keyPress('m')
        sleep(0.07)
        self.keyRelease('m')
        sleep(self.delay)
        down = H_state - 1 + 5
        for i in range(0, down):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('AU')
        sleep(self.delay)
        self.keyRelease('AU')
        sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        for i in range(0, 3):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('AU')
        sleep(self.delay)
        self.keyRelease('AU')
        sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        print("-> save Dinghy")
        return False

    def store_vehicle(self, H_state):
        self.keyPress('m')
        sleep(0.07)
        self.keyRelease('m')
        sleep(self.delay)
        down = H_state - 1 + 4
        for i in range(0, down):
            self.keyPress('AD')
            sleep(0.022)
            self.keyRelease('AD')
            sleep(0.022)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        for i in range(0, 4):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('m')
        sleep(self.delay)
        self.keyRelease('m')
        sleep(self.delay)
        print("-> store private vehicle")
        return False

    def mss_scan(self, data, bbox):
        sct_img = mss.mss().grab(bbox)  # type: ignore
        for key, item in data.items():
            pixel_list = list()
            cp = item['cp']
            pc = item['pc']
            if self.debug_pix:
                print("key =", key)
                print("cp =", cp)
                print("pc =", pc)
            for point in cp:
                pix = sct_img.pixel(point[0], point[1])
                pixel_list.append(pix)
            if self.debug_pix:
                print("PC =", pixel_list)
            if pc == pixel_list:
                return key
            else:
                continue
        return -1

    def ceo_mc_style(self, H_state, motorOrCEO):
        if H_state < 2 or motorOrCEO < 1:
            print("CEO나 MC를 먼저 등록해주세요.")
            return False, H_state, motorOrCEO
        elif H_state >= 2 and motorOrCEO == 1:
            self.keyPress('m')
            sleep(0.072)
            self.keyRelease('m')
            sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            for i in range(0, 2):
                self.keyPress('AD')
                sleep(self.delay)
                self.keyRelease('AD')
                sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            for i in range(0, 2):
                self.keyPress('AD')
                sleep(self.delay)
                self.keyRelease('AD')
                sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            sleep(0.052)
            self.keyPress('AR')
            sleep(0.022)
            self.keyRelease('AR')
            sleep(0.022)
            print("-> MC style ready")
            return False, H_state, motorOrCEO
        elif H_state >= 2 and motorOrCEO == 2:
            self.keyPress('m')
            sleep(0.072)
            self.keyRelease('m')
            sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')  # 관리
            sleep(self.delay)
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')  # 스타일
            sleep(0.052)
            self.keyPress('AR')
            sleep(0.022)
            self.keyRelease('AR')
            sleep(0.022)
            print("-> CEO style ready")
            return False, H_state, motorOrCEO
        else:
            return False, H_state, motorOrCEO

    def ceo_style(self, H_state, motorOrCEO, data, bb):
        num = 6
        if H_state == 1:  # nomal인 경우
            if num == 7:
                print("-> be the MC")
            else:
                print("-> be the CEO")
            self.keyPress('m')
            sleep(0.072)
            self.keyRelease('m')
            sleep(self.delay)
            for i in range(0, num):
                self.keyPress('AD')
                sleep(self.delay)
                self.keyRelease('AD')
                sleep(self.delay)
            for i in range(0, 2):
                self.keyPress('ET')
                sleep(self.delay)
                self.keyRelease('ET')
                sleep(self.delay)
            if num == 7:
                motorOrCEO = 1
            elif num == 6:
                motorOrCEO = 2
            H_state = 2
            print("H_state =", H_state)
            if motorOrCEO == 0:
                print("nomal state")
            elif motorOrCEO == 1:
                print("MC")
            elif motorOrCEO == 2:
                print("CEO")
            else:
                print("motorOrCEO =", motorOrCEO)
            sleep(0.2)
        elif motorOrCEO == 1:  # 모싸인 경우
            print("-> disassemble")
            self.keyPress('m')
            sleep(0.07)
            self.keyRelease('m')
            sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            self.keyPress('AU')
            sleep(self.delay)
            self.keyRelease('AU')
            sleep(0.2)
            res = self.mss_scan(data, bb)
            if res == 'check':
                print("진행중인 임무가 있으므로 CEO 스타일로 변경할 수 없습니다.")
                return False, H_state, motorOrCEO
            else:
                self.keyPress('ET')
                sleep(self.delay)
                self.keyRelease('ET')
                sleep(self.delay)
                if num == 7:
                    print("-> be the MC")
                elif num == 6:
                    print("-> be the CEO")
                sleep(0.08)
                self.keyPress('m')
                sleep(0.072)
                self.keyRelease('m')
                sleep(self.delay)
                for i in range(0, num):
                    self.keyPress('AD')
                    sleep(self.delay)
                    self.keyRelease('AD')
                    sleep(self.delay)
                for i in range(0, 2):
                    self.keyPress('ET')
                    sleep(self.delay)
                    self.keyRelease('ET')
                    sleep(self.delay)
                if num == 7:
                    motorOrCEO = 1
                elif num == 6:
                    motorOrCEO = 2
                H_state = 2
                print("H_state =", H_state)
                if motorOrCEO == 0:
                    print("nomal state")
                elif motorOrCEO == 1:
                    print("MC")
                elif motorOrCEO == 2:
                    print("CEO")
                else:
                    print("motorOrCEO =", motorOrCEO)
                sleep(0.2)
        else:
            pass
        self.keyPress('m')
        sleep(0.072)
        self.keyRelease('m')
        sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('AD')
        sleep(self.delay)
        self.keyRelease('AD')
        sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')  # 관리
        sleep(self.delay)
        self.keyPress('AD')
        sleep(self.delay)
        self.keyRelease('AD')
        sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')  # 스타일
        sleep(0.052)
        self.keyPress('AR')
        sleep(0.022)
        self.keyRelease('AR')
        sleep(0.022)
        print("-> CEO style up")
        return False, H_state, motorOrCEO

    def activate_thermal_mode(self, H_state):
        down = H_state - 1 + 3
        self.keyPress('m')
        sleep(0.072)
        self.keyRelease('m')
        sleep(self.delay)
        for i in range(0, down):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('AD')
        sleep(self.delay)
        self.keyRelease('AD')
        sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        for i in range(0, 4):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('SB')
        sleep(self.delay)
        self.keyRelease('SB')
        sleep(self.delay)
        self.keyPress('m')
        sleep(self.delay)
        self.keyRelease('m')
        sleep(self.delay)
        print("-> combat armor activate or deactivate thermal mode")
        return False

    def thermal_mode(self, H_state):
        down = H_state - 1 + 3
        self.keyPress('m')
        sleep(0.072)
        self.keyRelease('m')
        sleep(self.delay)
        for i in range(0, down):
            self.keyPress('AD')
            sleep(self.delay)
            self.keyRelease('AD')
            sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('AD')
        sleep(self.delay)
        self.keyRelease('AD')
        sleep(self.delay)
        self.keyPress('ET')
        sleep(self.delay)
        self.keyRelease('ET')
        sleep(self.delay)
        self.keyPress('SB')
        sleep(self.delay)
        self.keyRelease('SB')
        sleep(self.delay)
        self.keyPress('m')
        sleep(self.delay)
        self.keyRelease('m')
        sleep(self.delay)
        print("-> Thermal mode on/off")
        return False

    def check_inventory(self, data, bbox, data2, bbox2, data3, bb3):
        result = self.mss_scan(data, bbox)
        if result == 'check':
            self.keyPress('BS')
            sleep(self.delay)
            self.keyRelease('BS')
            sleep(self.delay)
            self.keyPress('AU')
            sleep(self.delay)
            self.keyRelease('AU')
            sleep(self.delay)
            self.keyPress('ET')
            sleep(self.delay)
            self.keyRelease('ET')
            sleep(self.delay)
            print("-> check bulletproof jacket")
            return False
        elif result == 'bp':
            self.keyPress('m')
            sleep(self.delay)
            self.keyRelease('m')
            sleep(self.delay)
            print("-> close menu")
            return False
        elif result == -1:
            self.keyPress('m')
            sleep(0.072)
            self.keyRelease('m')
            sleep(0.22)
            result2 = self.mss_scan(data2, bbox2)
            down = result2 - 1 + 2
            for i in range(0, down):
                self.keyPress('AD')
                sleep(self.delay)
                self.keyRelease('AD')
                sleep(self.delay)
            result3 = self.mss_scan(data3, bb3)
            if result3 == 'check':
                self.keyPress('ET')
                sleep(self.delay)
                self.keyRelease('ET')
                sleep(self.delay)
                for i in range(0, 4):
                    self.keyPress('AD')
                    sleep(self.delay)
                    self.keyRelease('AD')
                    sleep(self.delay)
                self.keyPress('ET')
                sleep(self.delay)
                self.keyRelease('ET')
                sleep(self.delay)
                print("-> check snacks")
                return False
            else:
                print("-> 상호작용 메뉴가 아니므로 작동 중지")
                return False
        else:
            return False

    def paste_text(self):
        print("-> paste text")
        text = clipboard.paste()
        print(text)
        self.keyPress('t')
        sleep(self.delay)
        self.keyRelease('t')
        sleep(0.2)
        self.keyPrint(text)
        return False
