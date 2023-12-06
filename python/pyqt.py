import sys
import time
from datetime import datetime
from math import sqrt

from PyQt5.QtWidgets import QApplication, QMainWindow, QMenu, QMenuBar, QAction, QWidget, QVBoxLayout, QHBoxLayout, \
    QLabel, QLineEdit, QPushButton, QMessageBox
from PyQt5.QtGui import QPainter, QColor, QBrush, QDoubleValidator, QFont
from PyQt5.QtCore import Qt, QTimer


class SimulationObject:
    def __init__(self, name="?", position=(-1.0, -1.0), velocity=(0.0, 0.0), acceleration=(0, 0), radius=0.0, mass=0.0):
        self.name = name
        self.position = position
        self.velocity = velocity
        self.acceleration = acceleration
        self.radius = radius
        self.mass = mass
        self.is_highlighted = False

    def fall(self, frame_time):
        self.acceleration = (0, 10)
        self.velocity = (
            self.velocity[0] + self.acceleration[0] * frame_time, self.velocity[1] + self.acceleration[1] * frame_time)
        self.position = (
            self.velocity[0] * frame_time + self.position[0], self.velocity[1] * frame_time + self.position[1])

    def reset_acceleration(self):
        self.acceleration = (0, 0)

    def collide(self, other):
        collision_normal = (
            other.position[0] - self.position[0],
            other.position[1] - self.position[1],
        )
        magnitude = sqrt(collision_normal[0] ** 2 + collision_normal[1] ** 2)
        if magnitude < self.radius + other.radius:
            magnitude = self.radius + other.radius
        collision_normal = (
            collision_normal[0] / magnitude, collision_normal[1] / magnitude)

        relative_velocity = (
            other.velocity[0] - self.velocity[0],
            other.velocity[1] - self.velocity[1],
        )

        impulse = (
                2.0
                * self.mass
                * other.mass
                * (
                        relative_velocity[0] * collision_normal[0]
                        + relative_velocity[1] * collision_normal[1]
                )
                / (self.mass + other.mass)
        )

        self.velocity = (
            self.velocity[0] + impulse * collision_normal[0] / self.mass,
            self.velocity[1] + impulse * collision_normal[1] / self.mass,
        )

        other.velocity = (
            other.velocity[0] - impulse * collision_normal[0] / other.mass,
            other.velocity[1] - impulse * collision_normal[1] / other.mass,
        )

    def simulate_step(self, frame_time):
        self.velocity = (
            self.velocity[0] + self.acceleration[0] * frame_time, self.velocity[1] + self.acceleration[1] * frame_time)
        self.position = (
            self.velocity[0] * frame_time + self.position[0], self.velocity[1] * frame_time + self.position[1])
        self.reset_acceleration()

    def apply_gravity(self, other, gforce):
        dist = sqrt((other.position[0] - self.position[0]) ** 2 + (other.position[1] - self.position[1]) ** 2)
        force = (gforce * self.mass * other.mass) / dist ** 2
        dir = (other.position[0] - self.position[0], other.position[1] - self.position[1])
        magnitude = sqrt(dir[0] ** 2 + dir[1] ** 2)
        dir_norm = (dir[0] / magnitude, dir[1] / magnitude)
        self.acceleration = (self.acceleration[0] + dir_norm[0] * force / self.mass,
                             self.acceleration[1] + dir_norm[1] * force / self.mass)
        other.acceleration = (other.acceleration[0] - dir_norm[0] * force / other.mass,
                              other.acceleration[1] - dir_norm[1] * force / other.mass)

    def detect_collision(self, other):
        if self == other:
            return False
        dist = sqrt((other.position[0] - self.position[0]) ** 2 + (other.position[1] - self.position[1]) ** 2)
        if dist < self.radius + other.radius:
            return True
        else:
            return False

class SimulationController:
    def __init__(self, app, size, margin):
        self.simulation_objects = []
        self.app = app
        self.size = size
        self.margin = margin
        self.prev_time = time.time()
        self.gforce = 6.67408
        self.isPaused = False
        self.simulation_speed = 1.0
        self.time_res = 0.0
        self.isAdding = True
        self.edited_object = None

    def reset_simulation(self):
        self.simulation_objects = []
        self.gforce = 6.67408
        self.prev_time = time.time()

    def brr(self):
        frame_time = time.time() - self.prev_time
        self.prev_time += frame_time
        if self.isPaused:
            return
        self.time_res += frame_time * self.simulation_speed
        steps_to_go = int(self.time_res//0.01)
        self.time_res -= steps_to_go * 0.01
        for _ in range(steps_to_go):
            self.next_frame(0.01)

    def next_frame(self, frame_time):
        self.simulate_gravity(frame_time, self.gforce)

    def check_destroy_object(self, o):
        destroy = False
        if o.position[0] < -self.margin[0]:
            destroy = True
        elif o.position[1] < -self.margin[1]:
            destroy = True
        elif o.position[0] > self.margin[0] + self.size[0]:
            destroy = True
        elif o.position[1] > self.margin[1] + self.size[1]:
            destroy = True
        return destroy

    def fall_all(self, frame_time):
        for o in self.simulation_objects:
            o.fall(frame_time)
            self.check_destroy_object(o)

    def simulate_gravity(self, frame_time, gforce):
        for i in range(len(self.simulation_objects)):
            for j in range(i + 1, len(self.simulation_objects)):
                self.simulation_objects[i].apply_gravity(self.simulation_objects[j], gforce)
                if self.simulation_objects[i].detect_collision(self.simulation_objects[j]):
                    self.app.set_info_label(
                        f"Kolizja obiektów {self.simulation_objects[i].name} i {self.simulation_objects[j].name}.")
                    self.simulation_objects[i].collide(self.simulation_objects[j])

        for o in self.simulation_objects:
            o.simulate_step(frame_time)
            if self.check_destroy_object(o):
                self.app.set_info_label(
                    f"Obiekt {o.name} opuścił obszar symulacji.")
                self.simulation_objects.remove(o)
                del o

    def highlight_object(self, o):
        self.unhighlight()
        o.is_highlighted = True

    def adjust_object(self, o):
        prev_radius = o.radius
        if self.app.radius_edit.text() != "":
            r = float(self.app.radius_edit.text().replace(",", "."))
            if r > 0:
                o.radius = r
            else:
                self.app.set_info_label("Zły format liczby")
                return
        for other in self.simulation_objects:
            if o.detect_collision(other):
                self.app.set_info_label("akcja spowodowałaby kolizję!")
                o.radius = prev_radius
                return
        if self.app.name_edit.text() != "":
            o.name = self.app.name_edit.text()
        if self.app.mass_edit.text() != "":
            r = float(self.app.mass_edit.text().replace(",", "."))
            if r > 0:
                o.mass = r
            else:
                self.app.set_info_label("Zły format liczby")
                return
        if self.app.velocity_edit_x.text() != "":
            r = float(self.app.velocity_edit_x.text().replace(",", "."))
            o.velocity = (r, o.velocity[1])
        if self.app.velocity_edit_y.text() != "":
            r = float(self.app.velocity_edit_y.text().replace(",", "."))
            o.velocity = (o.velocity[0], r)
        if self.app.position_edit_x.text() != "":
            r = float(self.app.position_edit_x.text().replace(",", "."))
            o.position = (r, o.position[1])
        if self.app.position_edit_y.text() != "":
            r = float(self.app.position_edit_y.text().replace(",", "."))
            o.position = (o.position[0], r)
        self.app.set_info_label(f"edytowano obiekt {o.name}")
        self.app.name_edit.setText("")
        self.app.mass_edit.setText("")
        self.app.radius_edit.setText("")
        self.app.velocity_edit_x.setText("")
        self.app.velocity_edit_y.setText("")
        self.app.position_edit_x.setText("")
        self.app.position_edit_y.setText("")

    def create_simulation_object(self, click_position):
        # create object
        # przypisanie właściwości na podstawie danych z panelu
        if self.app.name_edit.text() == "":
            self.app.set_info_label("uzupełnij pole 'nazwa'")
            return
        position = click_position
        mass = 100.0
        radius = 10.0
        velocity = (0.0, 0.0)

        if self.app.mass_edit.text() != "":
            mass = float(self.app.mass_edit.text().replace(",", "."))
            if mass <= 0:
                self.app.set_info_label("Zły format liczby")
                return
        if self.app.radius_edit.text() != "":
            radius = float(self.app.radius_edit.text().replace(",", "."))
            if radius <= 0:
                self.app.set_info_label("Zły format liczby")
                return
        if self.app.velocity_edit_x.text() != "":
            velocity = (float(self.app.velocity_edit_x.text().replace(",", ".")), velocity[1])
        if self.app.velocity_edit_y.text() != "":
            velocity = (velocity[0], float(self.app.velocity_edit_y.text().replace(",", ".")))
        if self.app.position_edit_x.text() != "":
            position = (float(self.app.position_edit_x.text().replace(",", ".")), position[1])
        if self.app.position_edit_y.text() != "":
            position = (position[0], float(self.app.position_edit_y.text().replace(",", ".")))
        o = SimulationObject(
            name=self.app.name_edit.text(),
            position=position,
            radius=radius,
            mass=mass,
            velocity=velocity)

        can_place = True
        for other in self.simulation_objects:
            if o.detect_collision(other):
                self.app.set_info_label("akcja spowodowałaby kolizję!")
                can_place = False
        if can_place:
            self.simulation_objects.append(o)
            self.app.add_object_tile(o)
            self.app.set_info_label(f"dodano obiekt {o.name}")
            self.app.name_edit.setText("")
            self.app.mass_edit.setText("")
            self.app.radius_edit.setText("")
            self.app.velocity_edit_x.setText("")
            self.app.velocity_edit_y.setText("")
            self.app.position_edit_x.setText("")
            self.app.position_edit_y.setText("")

    def choose_object_to_edit(self, o):
        self.app.set_info_label(f"wybrano obiekt {o.name}")
        self.highlight_object(o)
        self.edited_object = o

    def unhighlight(self):
        for other in self.simulation_objects:
            other.is_highlighted = False


class SimulationObjectTile(QWidget):
    def __init__(self, simulation_object, simulation_controller, wrapper):
        super().__init__()
        self.o = simulation_object
        self.controller = simulation_controller
        self.wrapper = wrapper
        self.setFixedWidth(380)
        self.setFixedHeight(50)
        self.tile_layout = QVBoxLayout(self)
        self.tile_layout.setContentsMargins(0, 0, 0, 0)
        self.tile_layout.setAlignment(Qt.AlignTop)
        self.upper_row = QWidget()
        self.upper_row_layout = QHBoxLayout(self.upper_row)
        self.upper_row_layout.setContentsMargins(0, 0, 0, 0)
        self.upper_row.setFixedSize(350, 40)
        self.upper_row_layout.setAlignment(Qt.AlignLeft)
        self.bottom_row = QWidget()
        self.bottom_row_layout = QHBoxLayout(self.bottom_row)
        self.bottom_row_layout.setContentsMargins(0, 0, 0, 0)
        self.bottom_row_layout.setAlignment(Qt.AlignLeft)
        self.button_name = QLabel(self.o.name)
        #font = self.button_name.font()  # Get the current font of the label
        #font.setPointSize(14)  # Set the desired font size
        #self.button_name.setFont(font)
        self.mass_label = QLabel("masa:")
        self.mass_val = QLabel(str(self.o.mass))
        self.position_label = QLabel("pozycja:")
        self.position_val = QLabel(f"{self.o.position[0]}; {self.o.position[1]}")
        self.position_val.setFixedWidth(300)
        self.speed_label = QLabel("prędkość:")
        self.speed_val = QLabel(f"{self.o.velocity[0]}; {self.o.velocity[1]}")
        self.speed_val.setFixedWidth(80)
        self.radius_label = QLabel("promień:")
        self.radius_val = QLabel(str(self.o.radius))
        self.acc_label = QLabel("przyspieszenie:")
        self.acc_val = QLabel(f"{self.o.acceleration[0]}; {self.o.acceleration[1]}")
        self.acc_val.setFixedWidth(80)
        self.upper_row_layout.addWidget(self.button_name)
        self.upper_row_layout.addWidget(self.mass_label)
        self.upper_row_layout.addWidget(self.mass_val)
        self.upper_row_layout.addWidget(self.speed_label)
        self.upper_row_layout.addWidget(self.speed_val)
        self.bottom_row_layout.addWidget(self.position_label)
        self.bottom_row_layout.addWidget(self.position_val)
        self.bottom_row_layout.addWidget(self.radius_label)
        self.bottom_row_layout.addWidget(self.radius_val)
        self.bottom_row_layout.addWidget(self.acc_label)
        self.bottom_row_layout.addWidget(self.acc_val)
        self.tile_layout.addWidget(self.upper_row)
        self.tile_layout.addWidget(self.bottom_row)

    def update(self):
        self.button_name.setText(self.o.name)
        self.mass_val.setText(str(self.o.mass))
        self.position_val.setText("{:.2f}; {:.2f}".format(self.o.position[0], self.o.position[1]))
        self.speed_val.setText("{:.2f}; {:.2f}".format(self.o.velocity[0], self.o.velocity[1]))
        self.radius_val.setText(str(self.o.radius))
        self.acc_val.setText("{:.2f}; {:.2f}".format(self.o.acceleration[0], self.o.acceleration[1]))
        if self.o not in self.controller.simulation_objects:
            self.remove()

    def mousePressEvent(self, event):
        self.controller.choose_object_to_edit(self.o)
        if self.controller.isAdding:
            self.controller.app.toggle_adding()

    def remove(self):
        self.controller.app.object_tiles.remove(self)
        if self.o in self.controller.simulation_objects:
            self.controller.simulation_objects.remove(self.o)
        self.deleteLater()
        self.controller.app.simulation_object_layout.removeWidget(self.wrapper)


class MainWindow(QMainWindow):
    def __init__(self):
        super().__init__()
        self.setWindowTitle("Symulator Grawitacji")
        self.setFixedSize(1150, 550)

        # Create simulation controller
        self.controller = SimulationController(self, (500, 500), (100, 100))

        # Create root widget
        self.root_widget = QWidget(self)
        self.setCentralWidget(self.root_widget)
        self.root_layout = QVBoxLayout(self.root_widget)
        self.root_layout.setContentsMargins(0, 0, 0, 0)
        self.root_widget.setLayout(self.root_layout)

        # Create main widget
        self.main_widget = QWidget(self)
        self.main_layout = QHBoxLayout(self.main_widget)
        self.main_layout.setContentsMargins(0, 0, 0, 0)

        # Create menu strip
        self.about_menu = QMenu("O aplikacji")
        self.menu = QMenuBar()
        self.menu.addMenu(self.about_menu)
        self.setMenuBar(self.menu)

        self.newAction = QAction(self)
        self.newAction.setText("Opis aplikacji:\nAplikacja służy do symulowania oddziaływań grawitacyjnych\
 pomiędzy obiektami o zadanych masach.\nZasymulowano również niecentralne odbicia idealnie sprężyste.")
#\n\
#Panel po lewej stronie aplikacji przedstawia dane pomiarowe na temat poszczególnych obiektów \
#uczestniczących w symulacji.\n\
#Wybranie obiektu umożliwia edycję jego właściwości w panelu po prawej stronie,\n \
#który służy również do ustalania właściwości nowych obiektów.\n\
#dodanie nowego obiektu, albo przesunięcie istniejącego, odbywa się \
#poprzez wskazanie myszą miejsca w obszarze symulacji.\n\
#Przyciski z napisem 'Tryb dodawania' lub 'Tryb edycji' służą jako przełączniki pomiędzy tymi trybami.")
        self.about_menu.addAction(self.newAction)



        self.menu_bar = QWidget(self)

        self.menu_bar_layout = QHBoxLayout(self.main_widget)
        self.menu_bar_layout.setContentsMargins(0, 0, 0, 0)

        # Create "About" menu and action
        #self.about_button = QPushButton("O aplikacji", self)
        #self.about_button.setFixedWidth(60)
        #self.about_button.clicked.connect(self.show_about_view)

        # Create "Info" label
        self.info_label = QLabel()
        self.set_info_label("Rozpoczęcie symulacji")
        self.info_label.setFixedWidth(300)

        # Create "Pause" button
        self.pause_button = QPushButton("Pauza", self)
        self.pause_button.setFixedWidth(60)
        self.pause_button.clicked.connect(self.toggle_pause)

        # Create "New simulation" button
        self.new_simulation_button = QPushButton("Nowa symulacja", self)
        self.new_simulation_button.setFixedWidth(100)
        self.new_simulation_button.clicked.connect(self.show_new_simulation_dialogue)

        # Create "Simulation speed" field
        self.sim_speed_label = QLabel("Prędkość symulacji:", self)
        self.sim_speed_label.setFixedWidth(100)
        self.sim_speed_field = QLineEdit("1")
        self.sim_speed_field.setFixedWidth(30)
        self.sim_speed_field.setValidator(QDoubleValidator())
        self.sim_speed_field.returnPressed.connect(self.change_simulation_speed)

        #self.menu_bar_layout.addWidget(self.about_button)
        self.menu_bar_layout.addWidget(self.info_label)
        self.menu_bar_layout.addWidget(self.pause_button)
        self.menu_bar_layout.addWidget(self.new_simulation_button)
        self.menu_bar_layout.addWidget(self.sim_speed_label)
        self.menu_bar_layout.addWidget(self.sim_speed_field)
        self.menu_bar_layout.setAlignment(Qt.AlignLeft)
        self.menu_bar.setLayout(self.menu_bar_layout)

        self.root_layout.addWidget(self.menu_bar)

        self.add_edit_button1 = QPushButton("➕ Tryb dodawania")
        self.add_edit_button1.clicked.connect(self.toggle_adding)
        self.add_edit_button2 = QPushButton("➕ Tryb dodawania")
        self.add_edit_button2.clicked.connect(self.toggle_adding)

        # Create simulation object's panel
        self.object_panel = QWidget(self)
        self.object_panel.setFixedWidth(400)
        self.simulation_object_layout = QVBoxLayout(self.object_panel)
        self.simulation_object_layout.setContentsMargins(0, 0, 0, 0)
        self.object_tiles = []
        self.simulation_object_layout.setAlignment(Qt.AlignTop)
        self.simulation_object_layout.addWidget(self.add_edit_button1)
        self.main_layout.addWidget(self.object_panel)

        # Create simulation area
        self.simulation_area = SimulationArea(self, self.controller)
        self.simulation_area.setFixedWidth(500)
        self.main_layout.addWidget(self.simulation_area)

        # Create object properties panel
        self.properties_panel = QWidget(self)
        self.properties_panel.setFixedWidth(200)
        self.properties_layout = QVBoxLayout(self.properties_panel)
        self.properties_layout.setContentsMargins(0, 0, 0, 0)

        self.name_label = QLabel("Nazwa:")
        self.name_edit = QLineEdit()
        self.mass_label = QLabel("Masa:")
        self.mass_edit = QLineEdit()
        self.mass_edit.setValidator(QDoubleValidator())
        self.radius_label = QLabel("Promień:")
        self.radius_edit = QLineEdit()
        self.radius_edit.setValidator(QDoubleValidator())

        self.velocity_label = QLabel("Prędkość:")
        self.velocity_edit_x = QLineEdit()
        self.velocity_edit_y = QLineEdit()
        self.velocity_edit_x.setValidator(QDoubleValidator())
        self.velocity_edit_y.setValidator(QDoubleValidator())
        self.velocity_label_x = QLabel("x:")
        self.velocity_label_y = QLabel("y:")
        self.velocity_edit_row = QWidget(self)
        self.velocity_edit_row.setLayout(QHBoxLayout())
        self.velocity_edit_row.layout().addWidget(self.velocity_label_x)
        self.velocity_edit_row.layout().addWidget(self.velocity_edit_x)
        self.velocity_edit_row.layout().addWidget(self.velocity_label_y)
        self.velocity_edit_row.layout().addWidget(self.velocity_edit_y)

        self.position_label = QLabel("Pozycja:")
        self.position_edit_x = QLineEdit()
        self.position_edit_y = QLineEdit()
        self.position_edit_x.setValidator(QDoubleValidator())
        self.position_edit_y.setValidator(QDoubleValidator())
        self.position_label_x = QLabel("x:")
        self.position_label_y = QLabel("y:")
        self.position_edit_row = QWidget(self)
        self.position_edit_row.setLayout(QHBoxLayout())
        self.position_edit_row.layout().addWidget(self.position_label_x)
        self.position_edit_row.layout().addWidget(self.position_edit_x)
        self.position_edit_row.layout().addWidget(self.position_label_y)
        self.position_edit_row.layout().addWidget(self.position_edit_y)

        self.properties_layout.addWidget(self.name_label)
        self.properties_layout.addWidget(self.name_edit)
        self.properties_layout.addWidget(self.mass_label)
        self.properties_layout.addWidget(self.mass_edit)
        self.properties_layout.addWidget(self.radius_label)
        self.properties_layout.addWidget(self.radius_edit)
        self.properties_layout.addWidget(self.velocity_label)
        self.properties_layout.addWidget(self.velocity_edit_row)
        self.properties_layout.addWidget(self.position_label)
        self.properties_layout.addWidget(self.position_edit_row)
        self.properties_layout.addWidget(self.add_edit_button2)

        self.name_edit.returnPressed.connect(self.adjust_edited)
        self.mass_edit.returnPressed.connect(self.adjust_edited)
        self.radius_edit.returnPressed.connect(self.adjust_edited)
        self.position_edit_x.returnPressed.connect(self.adjust_edited)
        self.position_edit_y.returnPressed.connect(self.adjust_edited)
        self.velocity_edit_x.returnPressed.connect(self.adjust_edited)
        self.velocity_edit_y.returnPressed.connect(self.adjust_edited)

        self.properties_layout.addStretch()
        self.main_layout.addWidget(self.properties_panel)

        # Create about view
        self.about_view = QWidget(self)
        self.about_view_layout = QVBoxLayout(self)
        self.about_view_layout.setAlignment(Qt.AlignCenter)
        self.about_view_text_label = QLabel("Opis aplikacji:\nAplikacja służy do symulowania oddziaływań grawitacyjnych\
 pomiędzy obiektami o zadanych masach.\nZasymulowano również niecentralne odbicia idealnie sprężyste.\n\
Panel po lewej stronie aplikacji przedstawia dane pomiarowe na temat poszczególnych obiektów \
uczestniczących w symulacji.\n\
Wybranie obiektu umożliwia edycję jego właściwości w panelu po prawej stronie,\n \
który służy również do ustalania właściwości nowych obiektów.\n\
dodanie nowego obiektu, albo przesunięcie istniejącego, odbywa się \
poprzez wskazanie myszą miejsca w obszarze symulacji.\n\
Przyciski z napisem 'Tryb dodawania' lub 'Tryb edycji' służą jako przełączniki pomiędzy tymi trybami.")
        self.about_view_text_label.setFixedWidth(1100)
        self.about_view_text_label.setWordWrap(True)
        self.about_view_back_button = QPushButton("Powrót", self)
        self.about_view_back_button.setFixedWidth(1100)
        self.about_view_back_button.clicked.connect(self.return_to_main)
        self.about_view_layout.addWidget(self.about_view_text_label)
        self.about_view_layout.addWidget(self.about_view_back_button)
        self.about_view.setLayout(self.about_view_layout)

        self.root_layout.addWidget(self.main_widget)
        self.root_layout.addWidget(self.about_view)
        self.about_view.setHidden(True)

    def adjust_edited(self):
        if not self.controller.isAdding:
            self.controller.adjust_object(self.controller.edited_object)

    def add_object_tile(self, o):
        wrapper = QWidget(self)
        tile = SimulationObjectTile(o, self.controller, wrapper)
        wrapper_layout = QHBoxLayout(wrapper)
        wrapper_layout.setAlignment(Qt.AlignLeft)
        wrapper_layout.setContentsMargins(0, 0, 0, 0)
        wrapper_layout.addWidget(tile)
        remove_button = QPushButton("X", self)
        remove_button.setFixedSize(40, 60)
        remove_button.clicked.connect(tile.remove)
        wrapper_layout.addWidget(remove_button)
        self.object_tiles.append(tile)
        self.simulation_object_layout.addWidget(wrapper)

    def show_about_view(self):
        self.main_widget.setHidden(True)
        self.about_view.setHidden(False)
        self.menu_bar.setHidden(True)

    def return_to_main(self):
        self.main_widget.setHidden(False)
        self.about_view.setHidden(True)
        self.menu_bar.setHidden(False)

    def set_info_label(self, text):
        current_time = datetime.now().strftime("%H:%M:%S:%f")[:-3]
        text = f"{current_time}: {text}"
        self.info_label.setText(text)

    def toggle_pause(self):
        if not self.controller.isPaused:
            self.pause_button.setText("Wznów")
            self.controller.isPaused = True
        else:
            self.pause_button.setText("Pauza")
            self.controller.isPaused = False

    def toggle_adding(self):
        if not self.controller.isAdding:
            self.add_edit_button1.setText("➕ Tryb dodawania")
            self.add_edit_button2.setText("➕ Tryb dodawania")
            self.controller.isAdding = True
            self.controller.unhighlight()
        else:
            if len(self.controller.simulation_objects) > 0:
                self.add_edit_button1.setText("✏️ Tryb edycji")
                self.add_edit_button2.setText("✏️ Tryb edycji")
                self.controller.isAdding = False
                self.controller.choose_object_to_edit(self.controller.simulation_objects[0])
            else:
                self.set_info_label("brak obiektów do edycji!")

    def show_new_simulation_dialogue(self):
        if not self.controller.isPaused:
            self.pause_button.setText("Wznów")
            self.controller.isPaused = True
        msgBox = QMessageBox()
        msgBox.setWindowTitle("Nowa symulacja")
        msgBox.setText("Czy na pewno chcesz rozpocząć nową symulację?")
        msgBox.setInformativeText("Obecny stan symulacji zostanie trwale usunięty.")
        msgBox.addButton("Rozpocznij nową symulację", QMessageBox.AcceptRole)
        msgBox.addButton("Zaniechaj", QMessageBox.RejectRole)

        result = msgBox.exec_()
        if result == QMessageBox.AcceptRole:
            self.controller.reset_simulation()
        elif result == QMessageBox.RejectRole:
            self.pause_button.setText("Pauza")
            self.controller.isPaused = False

    def change_simulation_speed(self):
        sender = self.sender()
        new_speed = float(sender.text().replace(",", "."))
        self.controller.simulation_speed = new_speed
        self.set_info_label(f"Nowa prędkość symulacji: {new_speed}")

    def update_tiles(self):
        for tile in self.object_tiles:
            tile.update()


class SimulationArea(QWidget):
    def __init__(self, parent, simulation_controller):
        super().__init__(parent)
        self.simulation_controller = simulation_controller

    def paintEvent(self, event):
        painter = QPainter(self)
        painter.setRenderHint(QPainter.Antialiasing)
        painter.setPen(Qt.transparent)
        painter.setBrush(QColor(151, 172, 184))  # Set the desired custom color
        painter.drawRect(self.rect())
        painter.setBrush(QBrush(Qt.gray, Qt.SolidPattern))
        font = QFont("Sans", 13)
        painter.setFont(font)
        for o in self.simulation_controller.simulation_objects:
            if o.is_highlighted:
                painter.setBrush(QBrush(Qt.white, Qt.SolidPattern))
            else:
                painter.setBrush(QBrush(QColor(220, 220, 220), Qt.SolidPattern))
            painter.drawEllipse(int(o.position[0] - o.radius), int(o.position[1] - o.radius), int(o.radius * 2),
                                int(o.radius * 2))
            painter.setPen(Qt.black)
            painter.drawText(int(o.position[0])+10, int(o.position[1])+10, o.name)
            painter.setPen(Qt.transparent)

    def mousePressEvent(self, event):
        if event.button() == Qt.LeftButton:
            x = event.x()
            y = event.y()
            if self.simulation_controller.isAdding:
                self.simulation_controller.create_simulation_object((x, y))
            else:
                prev_pos = self.simulation_controller.edited_object.position
                self.simulation_controller.edited_object.position = (x, y)
                for other in self.simulation_controller.simulation_objects:
                    if other.position == self.simulation_controller.edited_object.position:
                        continue
                    if self.simulation_controller.edited_object.detect_collision(other):
                        self.simulation_controller.app.set_info_label("akcja spowodowałaby kolizję!")
                        self.simulation_controller.edited_object.position = prev_pos
                        break

if __name__ == "__main__":
    app = QApplication(sys.argv)
    main_window = MainWindow()
    timer = QTimer()
    timer.timeout.connect(main_window.simulation_area.repaint)
    timer.timeout.connect(main_window.update_tiles)
    timer.timeout.connect(main_window.controller.brr)
    timer.start(1)
    main_window.show()
    sys.exit(app.exec_())
