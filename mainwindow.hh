/* Tetris
 *
 * Class: mainwindow
 * Class that creates ui and runs the tetris game.
 *
 * Program author
 * Name: Sijan Pandey
 * Student number: 293831
 * UserID: xnsipa
 * E-Mail: sijan.pandey@tuni.fi
 * */

#ifndef MAINWINDOW_HH
#define MAINWINDOW_HH

#include <QMainWindow>
#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <random>
#include <QVector>
#include <QTimer>
#include <fstream>
#include <iostream>
#include <string>

namespace Ui {
class MainWindow;
}

// initial timer interval for clock and game speed
const int INITIAL_GAME_TIME = 1000;

// save file name for user data
const std::string SAVE_FILE = "Scores.txt";

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void keyPressEvent(QKeyEvent* event) override;

private slots:
    /**
     * @brief on_start_button_clicked event listener for start button click
     */
    void on_start_button_clicked();

    /**
     * @brief time_update listens to its own timer and updates time on screen
     */
    void time_update();

    /**
     * @brief game_update listens to its own timer and updates scene
     */
    void game_update();


private:
    Ui::MainWindow *ui;

    QGraphicsScene* scene_;

    // timer for speed of tetrimino
    QTimer* game_timer_;

    // timer for updating on screen clock
    QTimer* clock_timer_;

    // Constants describing scene coordinates
    // Copied from moving circle example and modified a bit
    const int BORDER_UP = 0;
    const int BORDER_DOWN = 480; // 260; (in moving circle)
    const int BORDER_LEFT = 0;
    const int BORDER_RIGHT = 240; // 680; (in moving circle)


    // Size of a tetromino component
    const int SQUARE_SIDE = 20;
    // Number of horizontal cells (places for tetromino components)
    const int COLUMNS = BORDER_RIGHT / SQUARE_SIDE;
    // Number of vertical cells (places for tetromino components)
    const int ROWS = BORDER_DOWN / SQUARE_SIDE;

    // Constants for different tetrominos and the number of them
    enum Tetromino_kind {HORIZONTAL,
                         LEFT_CORNER,
                         RIGHT_CORNER,
                         SQUARE,
                         STEP_UP_RIGHT,
                         PYRAMID,
                         STEP_UP_LEFT,
                         NUMBER_OF_TETROMINOS};
    // From the enum values above, only the last one is needed in this template.
    // Recall from enum type that the value of the first enumerated value is 0,
    // the second is 1, and so on.
    // Therefore the value of the last one is 7 at the moment.
    // Remove those tetromino kinds above that you do not implement,
    // whereupon the value of NUMBER_OF_TETROMINOS changes, too.
    // You can also remove all the other values, if you do not need them,
    // but most probably you need a constant value for NUMBER_OF_TETROMINOS.


    // For randomly selecting the next dropping tetromino
    std::default_random_engine randomEng;
    std::uniform_int_distribution<int> distr;

    // Vector that stores each cell of a tetromino
    QVector<QGraphicsRectItem*> tetromino;

    // Vector that stores color values
    const QVector<QColor> COLORS = {Qt::cyan, Qt::blue, Qt::gray, Qt::yellow,
                                    Qt::green, Qt::magenta, Qt::red,
                                    Qt::black};

    // initial coordinates for tetrominos
    const int coords_table[NUMBER_OF_TETROMINOS][4][2] = {
             { { SQUARE_SIDE*0, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*1, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*2, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*3, SQUARE_SIDE*-1 } },
             { { SQUARE_SIDE*0, SQUARE_SIDE*-2 },
               { SQUARE_SIDE*0, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*1, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*2, SQUARE_SIDE*-1 } },
             { { SQUARE_SIDE*0, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*1, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*2, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*2, SQUARE_SIDE*-2 } },
             { { SQUARE_SIDE*0, SQUARE_SIDE*-2 },
               { SQUARE_SIDE*1, SQUARE_SIDE*-2 },
               { SQUARE_SIDE*0, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*1, SQUARE_SIDE*-1 } },
             { { SQUARE_SIDE*0, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*1, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*1, SQUARE_SIDE*-2 },
               { SQUARE_SIDE*2, SQUARE_SIDE*-2 } },
             { { SQUARE_SIDE*0, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*1, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*1, SQUARE_SIDE*-2 },
               { SQUARE_SIDE*2, SQUARE_SIDE*-1 } },
             { { SQUARE_SIDE*0, SQUARE_SIDE*-2 },
               { SQUARE_SIDE*1, SQUARE_SIDE*-2 },
               { SQUARE_SIDE*1, SQUARE_SIDE*-1 },
               { SQUARE_SIDE*2, SQUARE_SIDE*-1 } }
         };

    // Vector for tetrominos that have stopped moving
    QVector<QVector<QGraphicsRectItem*>> old_tetrominos;

    // Keeps track of game time
    int game_time;

    // Game points based on number of fallen tetrominos in game area
    int points;

    // game has ended or not
    bool game_end;

    // time interval for timers
    int timer_param;

    // username of the player
    std::string username;

    /**
     * @brief create_tetromino create a new tetromino
     * @param shape int value of shape to be created randomly selected from enum
     */
    void create_tetromino(int shape);

    /**
     * @brief update_scene renders change in game area if move is legal
     * @param horizontal_step step tetrimino has to move horizontally
     * @param vertical_step step tetrimino has to move vertically
     */
    void update_scene(qreal horizontal_step, qreal vertical_step);

    /**
     * @brief check_move check whether next move is legal
     * @param horizontal_step step tetrimino has to move horizontally
     * @param vertical_step step tetrimino has to move vertically
     * @return bool value for whether legal move or not
     */
    bool check_move(qreal horizontal_step, qreal vertical_step);

    /**
     * @brief save_game_data save the user game data in file for future
     */
    void save_game_data();

    /**
     * @brief end_game renders ui and fixes settings for case of end of game
     * reached
     */
    void end_game();
};

#endif // MAINWINDOW_HH
