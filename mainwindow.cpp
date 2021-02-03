#include "mainwindow.hh"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    // We need a graphics scene in which to draw rectangles.
    scene_ = new QGraphicsScene(this);

    // The graphicsView object is placed on the window
    // at the following coordinates, but if you want
    // different placement, you can change their values.
    int left_margin = 350; // x coordinate
    int top_margin = 10; // y coordinate

    // The width of the graphicsView is BORDER_RIGHT added by 2,
    // since the borders take one pixel on each side
    // (1 on the left, and 1 on the right).
    // Similarly, the height of the graphicsView is BORDER_DOWN added by 2.
    ui->graphicsView->setGeometry(left_margin, top_margin,
                                  BORDER_RIGHT + 2, BORDER_DOWN + 2);
    ui->graphicsView->setScene(scene_);

    // The width of the scene_ is BORDER_RIGHT decreased by 1 and
    // the height of it is BORDER_DOWN decreased by 1, because
    // each square of a tetromino is considered to be inside the sceneRect,
    // if its upper left corner is inside the sceneRect.
    scene_->setSceneRect(0, 0, BORDER_RIGHT - 1, BORDER_DOWN - 1);

    // Setting random engine ready for the first real call.
    int seed = time(0); // You can change seed value for testing purposes
    randomEng.seed(seed);
    distr = std::uniform_int_distribution<int>(0, NUMBER_OF_TETROMINOS - 1);
    distr(randomEng); // Wiping out the first random number (which is almost always 0)
    // After the above settings, you can use randomEng to draw the next falling
    // tetromino by calling: distr(randomEng) in a suitable method.

    // timer slot settings
    game_timer_ = new QTimer(this);
    connect(game_timer_, SIGNAL(timeout()),
                  this, SLOT(game_update()));
    clock_timer_ = new QTimer(this);
    connect(clock_timer_, SIGNAL(timeout()),
                  this, SLOT(time_update()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete game_timer_;
    delete clock_timer_;
    delete scene_;
}

void MainWindow::keyPressEvent(QKeyEvent* event)
{
    if(!game_end)
    {
        // moving the tetromino to left by one step
        if(event->key() == Qt::Key_A) {
            qreal horizontal_step = static_cast<qreal>(-SQUARE_SIDE);
            qreal vertical_step = static_cast<qreal>(0);
            update_scene(horizontal_step, vertical_step);
        }

        // moving the tetromino to down by one step
        if(event->key() == Qt::Key_S) {
            qreal horizontal_step = static_cast<qreal>(0);
            qreal vertical_step = static_cast<qreal>(SQUARE_SIDE);
            update_scene(horizontal_step, vertical_step);
        }

        // moving the tetromino to right by one step
        if(event->key() == Qt::Key_D) {
            qreal horizontal_step = static_cast<qreal>(SQUARE_SIDE);
            qreal vertical_step = static_cast<qreal>(0);
            update_scene(horizontal_step, vertical_step);
        }
    }
}

void MainWindow::on_start_button_clicked()
{
    // initial settings for the game, applies to restarting a new game also
    scene_->clear();
    old_tetrominos.clear();
    timer_param = INITIAL_GAME_TIME;
    points = 0;
    game_time = 0;
    ui->score_lcd->display(points);
    ui->time_lcd->display(game_time);
    game_end = false;

    game_timer_->start(timer_param);
    clock_timer_->start(timer_param);

    ui->start_button->setDisabled(true);
    ui->username_edit->setDisabled(true);

    create_tetromino(distr(randomEng));
}

void MainWindow::time_update()
{
    game_time++;
    ui->time_lcd->display(game_time);
}

void MainWindow::game_update()
{
    qreal horizontal_step = static_cast<qreal>(0);
    qreal vertical_step = static_cast<qreal>(SQUARE_SIDE);
    update_scene(horizontal_step, vertical_step);
}

void MainWindow::update_scene(qreal horizontal_step, qreal vertical_step)
{
    // if the move is legal then render it on the screen
    if(check_move(horizontal_step, vertical_step))
    {
        for(int i=0; i < tetromino.size();i++)
        {
            tetromino.at(i)->moveBy(horizontal_step, vertical_step);
        }
    }
}

bool MainWindow::check_move(qreal horizontal_step, qreal vertical_step)
{
    bool illegal_move_block = false;
    bool illegal_move_horizontal = false;
    bool illegal_move_vertical = false;
    int num_cells_in_scene = 0;

    // get how many cells are still outside game area
    for(int i = 0; i < 4; i++){
        if(scene_->sceneRect().contains(tetromino.at(i)->x(),
                                        tetromino.at(i)->y()))
        {
            num_cells_in_scene++;
        }
    }

    // check whether the next move is legal or not
    for(int i = 0; i < 4; i++){
        qreal new_x = tetromino.at(i)->x() + horizontal_step;
        qreal new_y = tetromino.at(i)->y() + vertical_step;

        // checking possibility of move for horizontal movement being within
        // the scene
        if(new_x < BORDER_LEFT or new_x > BORDER_RIGHT - 1){
            illegal_move_horizontal = true;
        }
        if(new_y > BORDER_DOWN - 1){
            illegal_move_vertical = true;
        }

        // checking whether there is old tetromino in the place of next move
        for(int j=0; j < old_tetrominos.size();j++){
            for(int k=0; k < old_tetrominos.at(j).size(); k++){
                qreal reserved_x = old_tetrominos.at(j).at(k)->x();
                qreal reserved_y = old_tetrominos.at(j).at(k)->y();

                if(new_x == reserved_x and new_y == reserved_y){
                    illegal_move_block = true;
                }
            }
        }

        // trying to move out of the game area horizontally
        if(illegal_move_horizontal){
            return false;
        }

        if(num_cells_in_scene == 4){
            // horizontal movement blocked by old tetromino
            if(illegal_move_block and vertical_step == 0){
                return false;
            }

            // tetromino reaches the end or lands on top of old tetromino and
            // new tetromino is created
            if(illegal_move_vertical or illegal_move_block)
            {
                old_tetrominos.push_back(tetromino);
                create_tetromino(distr(randomEng));
                points++;
                ui->score_lcd->display(points);
                if(timer_param >= 150)
                {
                    // every tetromino moves faster by 50 ms than previous one
                    // 100 ms is the fastest interval for the timer
                    timer_param -= 50;
                    game_timer_->setInterval(timer_param);
                }

                return false;
            }
        }
        else{
            // game ends when tetromino is just released and is fully or
            // partially not in scene and lands on a old tetromino
            if(illegal_move_block){
                end_game();
                save_game_data();
                return false;
            }
        }
    }
    return true;
}

void MainWindow::save_game_data()
{
    username = ui->username_edit->toPlainText().toStdString();
    std::ofstream out_file_object(SAVE_FILE, std::ios_base::app);
    out_file_object << username << ", " << points << " points, " << game_time <<
                       " sec" << std::endl;
}

void MainWindow::end_game()
{
    game_timer_->stop();
    clock_timer_->stop();
    ui->start_button->setDisabled(false);
    ui->username_edit->setDisabled(false);
    ui->game_end_label->setText("Game Over!");
    game_end = true;
}

void MainWindow::create_tetromino(int tetromino_shape)
{
    tetromino.clear();

    // Color settings for the tetromino, color based on shape of tetromino
    QBrush brush(COLORS.at(tetromino_shape));
    QPen black_pen(COLORS.at(7));
    black_pen.setWidth(0);

    QGraphicsRectItem* rect;

    // For random initial position of falling for tetromino
    int seed = time(0);
    std::default_random_engine random;
    std::uniform_int_distribution<int> distrib;
    random.seed(seed);
    distrib = std::uniform_int_distribution<int>(0, COLUMNS-4);
    distrib(random);
    int random_disp = distrib(random);

    // Create each square cell for the tetromino and save the combination to a vector
    for(int i=0; i < 4; i++)
    {
        rect = scene_->addRect(
                    0,
                    0,
                    SQUARE_SIDE,
                    SQUARE_SIDE,
                    black_pen,
                    brush);
        rect->moveBy(coords_table[tetromino_shape][i][0]
                + random_disp * SQUARE_SIDE,
                coords_table[tetromino_shape][i][1]);
        tetromino.push_back(rect);
    }
}



