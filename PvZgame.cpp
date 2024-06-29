#include "lab_m1/PvZgame/PvZgame.h"

#include <vector>
#include <iostream>

//random
#include <stdlib.h>     /* srand, rand */


#include "lab_m1/PvZgame/transform_2D.h"
#include "lab_m1/PvZgame/object_pvzgame.h"

using namespace std;
using namespace m1;

float radians, translateX, signT = 1, scaleX = 1, scaleY = 1, signS = 1;
float squareSide = 100;
glm::vec3 corner = glm::vec3(0, 0, 0);
const float length = 10.0f; 
vector<glm::vec3> diamondCenters;
vector<glm::vec3> diamondActiveCenters;
int highScore = 0;

vector<glm::vec3> leftCornersGreenSquares;
bool dragging;
int draggedDiamondIndex = -1;
int moving = 0;
float squareSideLife = 88;
int dOverSquareID;
std::vector<bool> isSquareOccupied(9, false);
int readyToPlace[9][4];

std::vector<Mesh*> stars; // Stores all currently visible stars
std::vector<Mesh*> bulletStars;
float spawnRate = 4.0f; // Average time in seconds to spawn a new star
float lastSpawnTime = 0.0f; // When the last star was spawned
float currentTime = 0.0f; // Current time in the game
glm::vec3 randomPosition;
int scaledDown[9];
float scaleValues[9] = { 1,1,1,1,1,1,1,1,1 };
//vector with star centers
vector<glm::vec3> starCenters;
int star_count = 0;
int diamond_cost[4];
vector <glm::vec3> hexagonCenters;
int cnt = 0;

float starSpawnTimer = 0.0f;
float starSpawnRate = 2.0f; // Shoot a star every 2 seconds
float star_speed = 30;


float starSpawnTimers[9]= { 0,0,0,0,0,0,0,0,0 };

int readyToShoot[9][4];

float hexagonSpawnRate = 6.0f;
float hexagonTimer = 0.0f;
int lives = 3;
struct HexagonEnemy {
    glm::vec3 position;
    float speed;
    std::string meshID;
    glm::vec3 color;
    int hits = 0;
    float scaleValues = 1;
    bool isScalingDown;
    float scaleSpeed; // Speed at which the hexagon scales down
};

struct ShootingStar {
    glm::vec3 position;
    glm::vec3 position_copy;
    glm::vec3 color;
    float speed;
    float star_angle;
    Mesh* mesh;
};
std::vector<HexagonEnemy> hexagon_enemies;
std::vector<ShootingStar> shootingStars;

/*
 *  To find out more about `FrameStart`, `Update`, `FrameEnd`
 *  and the order in which they are called, see `world.cpp`.
 */


PvZgame::PvZgame()
{
}


PvZgame::~PvZgame()
{
}

float RandomRange(float min, float max) {
    return min + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (max - min)));
}

void PvZgame::Init()
{
    glm::ivec2 resolution = window->GetResolution();
    auto camera = GetSceneCamera();
    camera->SetOrthographic(0, (float)resolution.x, 0, (float)resolution.y, 0.01f, 400);
    camera->SetPosition(glm::vec3(0, 0, 20));
    camera->SetRotation(glm::vec3(0, 0, 0));
    camera->Update();
    GetCameraInput()->SetActive(false);

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 120;
    float diamondSide = 35;
    bool dragging = false;
    diamond_cost[0] = 1;
    diamond_cost[1] = 2;
    diamond_cost[2] = 2;
    diamond_cost[3] = 3;

    
    float cx = corner.x + squareSide / 2;
    float cy = corner.y + squareSide / 2;
    // Initialize tx and ty (the translation steps)
    translateX = 0;
    translateY = 0;

    // Initialize sx and sy (the scale factors)
    scaleX = 1;
    scaleY = 1;

    // Initialize angularStep
    angularStep = 0;

    // Gap for GUI and reduced spacing between squares
    float gap_for_GUI = 240.0f;   
    float reduced_spacing = 2.0f;   

    // Starting positions for the red rectangle and the top-left green square
    float startX = 10.0f;
    float startY = 10 - gap_for_GUI;

    float topMargin = 30; // Space from the top of the window to the GUI

    // Coordinates for the first GUI square
    float guiStartX = 10;
    float guiStartY = resolution.y - topMargin - squareSide;

    // Drawing red rectangle
    for (int i = 0; i < 3; i++) {
        glm::vec3 redSquareCorner = glm::vec3(guiStartX + 4 * (squareSide + 20) + 50 + i * (squareSideLife + 30), guiStartY + 25, 0);
        Mesh* redSquare = object2D::CreateSquare("redSquare" + to_string(i), redSquareCorner, squareSideLife, glm::vec3(1, 0, 0), true);
        AddMeshToList(redSquare);
    }

    // Create the red rectangle
    glm::vec3 redRectCorner = glm::vec3(0, 20, 0);
    float redRectWidth = 50;
    float redRectHeight = resolution.y - gap_for_GUI; // it spans the entire height
    Mesh* redRect = object2D::CreateRectangle("redRect", redRectCorner, redRectWidth, redRectHeight, glm::vec3(1, 0, 0), true);
    AddMeshToList(redRect);

    startX += redRectWidth + reduced_spacing;


    // Drawing the 3x3 grid of green squares
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int k;
            glm::vec3 greenSquareCorner = glm::vec3(70 + j * (squareSide + 30), 50 + i * (squareSide + 30), 0);
            leftCornersGreenSquares.push_back(greenSquareCorner);
            Mesh* greenSquare = object2D::CreateSquare("greenSquare" + to_string(i) + to_string(j), greenSquareCorner, squareSide, glm::vec3(0, 1, 0), true);
            AddMeshToList(greenSquare);
        }
    }


    for (int i = 0; i < 4; i++) {
        // GUI square corner
        glm::vec3 guiSquareCorner = glm::vec3(guiStartX + i * (squareSide + 20), guiStartY, 0);
        Mesh* guiSquare = object2D::CreateSquare("guiSquare" + to_string(i), guiSquareCorner, squareSide, glm::vec3(1, 1, 1), false); // White color, not filled for empty square
        AddMeshToList(guiSquare);

      

        float r, g, b;
        if (i == 0) {
            r = 0.7;
            g = 0;
            b = 1;
        }
        else if (i == 1) {
            r = 0.8;
            g = 0.3;
            b = 0.2;
        }
        else if (i == 2) {
            r = 0;
            g = 0;
            b = 1;
        }
        else {
            r = 1;
            g = 0.5;
            b = 0;
        }

        // Diamond inside GUI square
        glm::vec3 leftDiamondCorner = glm::vec3(guiStartX + i * (squareSide + 20) + 7, guiStartY + 60, 5); // Center of the square
        //add centers to vector
        glm::vec3 DiamondCenter = glm::vec3(leftDiamondCorner.x + diamondSide, leftDiamondCorner.y, 5);
        diamondCenters.push_back(DiamondCenter);
        Mesh* diamond = object2D::CreateDiamond("guiDiamond" + to_string(i), leftDiamondCorner, diamondSide, glm::vec3(r, g, b), true); // Red color, filled diamond
        AddMeshToList(diamond);

    }
    //add a star under first square
    glm::vec3 starCenter2 = glm::vec3(guiStartX + 0 * (squareSide + 20) + 20 + 0 * 25, guiStartY - 25, 0);
    float outerRadius = 18;
    float innerRadius = 9;
    Mesh* star = object2D::CreateStar("star" + to_string(0) + to_string(0), starCenter2, outerRadius, innerRadius, glm::vec3(0.5, 0.5, 0.5), true);
    AddMeshToList(star);

    //add 2 stars under the second square
    for (int i = 0;i < 2;i++)
    {
        glm::vec3 starCenter2 = glm::vec3(guiStartX + 1 * (squareSide + 20) + 20 + i * 30, guiStartY - 25, 0);
        float outerRadius = 18;
        float innerRadius = 9;
        Mesh* star = object2D::CreateStar("star" + to_string(1) + to_string(i), starCenter2, outerRadius, innerRadius, glm::vec3(0.5, 0.5, 0.5), true);
        AddMeshToList(star);
    }

    //add 2 stars under the third square
    for (int i = 0;i < 2;i++)
    {
        glm::vec3 starCenter2 = glm::vec3(guiStartX + 2 * (squareSide + 20) + 20 + i * 30, guiStartY - 25, 0);
        float outerRadius = 18;
        float innerRadius = 9;
        Mesh* star = object2D::CreateStar("star" + to_string(2) + to_string(i), starCenter2, outerRadius, innerRadius, glm::vec3(0.5, 0.5, 0.5), true);
        AddMeshToList(star);
    }

    //add 3 stars under the 4th square
    for (int i = 0;i < 3;i++)
    {
        glm::vec3 starCenter2 = glm::vec3(guiStartX + 3 * (squareSide + 20) + 20 + i * 30, guiStartY - 25, 0);
        float outerRadius = 18;
        float innerRadius = 9;
        Mesh* star = object2D::CreateStar("star" + to_string(3) + to_string(i), starCenter2, outerRadius, innerRadius, glm::vec3(0.5, 0.5, 0.5), true);
        AddMeshToList(star);

    }

    //create 9 * 4 diamonds to place over green squares
    //keep colors from gui diamonds
    for (int i = 0;i < 9;i++)
    {
        for (int j = 0;j < 4;j++)
        {
            glm::vec3 leftDiamondCorner = glm::vec3(leftCornersGreenSquares[i].x + 7, leftCornersGreenSquares[i].y + 60, 5); // Center of the square
            glm::vec3 DiamondCenter = glm::vec3(leftDiamondCorner.x + diamondSide, leftDiamondCorner.y, 5);
            diamondActiveCenters.push_back(DiamondCenter);
            float r, g, b;
            if (j == 0) {
                r = 0.7;
                g = 0;
                b = 1;
            }
            else if (j == 1) {
                r = 0.8;
                g = 0.3;
                b = 0.2;
            }
            else if (j == 2) {
                r = 0;
                g = 0;
                b = 1;
            }
            else {
                r = 1;
                g = 0.5;
                b = 0;
            }
            Mesh* diamond = object2D::CreateDiamond("diamond" + to_string(i) + to_string(j), leftDiamondCorner, diamondSide, glm::vec3(r, g, b), true); // Red color, filled diamond
            AddMeshToList(diamond);
        }
    }



    //if erased a star, add it to the right of the GUI, under the red squares
    //max 20 stars on screen
    for (int i = 0;i < 30;i++)
    {
        glm::vec3 starCenter = glm::vec3(guiStartX + 4 * (squareSide + 20) + 50 + i * 30, guiStartY, 0);
        float outerRadius = 17;
        float innerRadius = 8.5;
        Mesh* star = object2D::CreateStar("stars_money" + to_string(i), starCenter, outerRadius, innerRadius, glm::vec3(0.5, 0.5, 0.5), true);
        AddMeshToList(star);

    }



}



void PvZgame::FrameStart()
{
    // Clears the color buffer (using the previously set color) and depth buffer
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glm::ivec2 resolution = window->GetResolution();
    // Sets the screen area where to draw
    glViewport(0, 0, resolution.x, resolution.y);
}


void PvZgame::Update(float deltaTimeSeconds)
{

    glm::vec3 corner = glm::vec3(0, 0, 0);
    float squareSide = 100;
    currentTime += deltaTimeSeconds;
    //int maxStarsOnScreen;
    if ((currentTime - lastSpawnTime) > spawnRate) {
        spawnRate = RandomRange(2, 7);
        randomPosition = glm::vec3(RandomRange(500, 1270), RandomRange(15, 600), 19);
        //printf("Random position: %f, %f\n", randomPosition.x, randomPosition.y);
        starCenters.push_back(randomPosition);
        stars.push_back(object2D::CreateStar("star", randomPosition, 24, 13, glm::vec3(0.5, 0.5, 1), true)); // Fill in the rest of the parameters
    
        lastSpawnTime = currentTime;
    }

    for (Mesh* star : stars) {
        modelMatrix = glm::mat3(1);
        RenderMesh2D(star, shaders["VertexColor"], modelMatrix);
    }



    //spawm hexagons on random rows at random times

    hexagonTimer += deltaTimeSeconds;
    if (hexagonTimer >= hexagonSpawnRate) {
        hexagonTimer = 0.0f;
        hexagonSpawnRate = RandomRange(4, 10);

        int randomRow = rand() % 3;
        float y;
        if (randomRow == 0) {
             y = 110;
        }
        else if (randomRow == 1) {
			 y = 260;
		}
        else {
			 y = 410;
		}

        //choose another random number between 1 and 4
        int randomHexagon = rand() % 4;
        float r, g, b;
        if (randomHexagon == 0) {
			r = 0.7;
			g = 0;
			b = 1;
		}
        else if (randomHexagon == 1) {
			r = 0.8;
			g = 0.3;
			b = 0.2;
		}
        else if (randomHexagon == 2) {
			r = 0;
			g = 0;
			b = 1;
		}
        else if (randomHexagon == 3) {
			r = 1;
			g = 0.5;
			b = 0;
		}
        cnt++;
        //create hexagon
        glm::vec3 hexagonCenter = glm::vec3(0, 0, 12);

        std::string hexID1 = "hexagon1" + to_string(cnt);

        Mesh* hexagon1 = object2D::CreateHexagon(hexID1, hexagonCenter, 35, glm::vec3(r, g, b), true);
       AddMeshToList(hexagon1);
        glm::vec3 hexagonCenter2 = glm::vec3(0, 0, 15);
        Mesh* hexagon2 = object2D::CreateHexagon("hexagon2", hexagonCenter2, 35 / 2, glm::vec3(0.7, 0.8, 0.1), true);
        AddMeshToList(hexagon2);
        
        HexagonEnemy hexagon;
        hexagon.position = glm::vec3(1270, y, 0);
        hexagon.speed = 50;
        hexagon.meshID = hexID1;
        hexagon.color = glm::vec3(r, g, b);
        hexagon.scaleValues = 1;
        hexagon.isScalingDown = false;
        hexagon.scaleSpeed = 2;
        hexagon_enemies.push_back(hexagon);

    }

    for (auto& it : hexagon_enemies) {
        it.position.x -= deltaTimeSeconds * it.speed;
    }

    for (int i = 0;i < hexagon_enemies.size();i++) {
        if (hexagon_enemies[i].isScalingDown == false) {
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(hexagon_enemies[i].position.x, hexagon_enemies[i].position.y);
            RenderMesh2D(meshes[hexagon_enemies[i].meshID], shaders["VertexColor"], modelMatrix);
            modelMatrix = glm::mat3(1);
            modelMatrix *= transform2D::Translate(hexagon_enemies[i].position.x, hexagon_enemies[i].position.y);
            RenderMesh2D(meshes["hexagon2"], shaders["VertexColor"], modelMatrix);
        }
        else
        {
            hexagon_enemies[i].scaleValues -= hexagon_enemies[i].scaleSpeed * deltaTimeSeconds;
            if (hexagon_enemies[i].scaleValues <= 0) {
				// Scale has reached zero, remove the hexagon from the list
				//remove the hexagon from the list
                if (hexagon_enemies[i].color == glm::vec3(0.7, 0, 1))
                {
					highScore += 1;
				}
                if (hexagon_enemies[i].color == glm::vec3(0.8, 0.3, 0.2))
                {
                    highScore += 3;
                }
                if (hexagon_enemies[i].color == glm::vec3(0, 0, 1))
                {
					highScore += 3;
				}
                if (hexagon_enemies[i].color == glm::vec3(1, 0.5, 0))
                {
                    highScore += 5;
                }
                hexagon_enemies.erase(std::remove_if(hexagon_enemies.begin(), hexagon_enemies.end(), [&i](const HexagonEnemy& hex) {
					return hex.position.x == hexagon_enemies[i].position.x; 
					}),
                    					hexagon_enemies.end()
                				);
				continue;
			}
			modelMatrix = glm::mat3(1);
			modelMatrix *= transform2D::Translate(hexagon_enemies[i].position.x, hexagon_enemies[i].position.y);
			modelMatrix *= transform2D::Scale(hexagon_enemies[i].scaleValues, hexagon_enemies[i].scaleValues);
			RenderMesh2D(meshes[hexagon_enemies[i].meshID], shaders["VertexColor"], modelMatrix);
			modelMatrix = glm::mat3(1);
			modelMatrix *= transform2D::Translate(hexagon_enemies[i].position.x, hexagon_enemies[i].position.y);
			modelMatrix *= transform2D::Scale(hexagon_enemies[i].scaleValues, hexagon_enemies[i].scaleValues);
			RenderMesh2D(meshes["hexagon2"], shaders["VertexColor"], modelMatrix);
        }
   
    }

 
    for (auto& it : hexagon_enemies) {

        if (it.position.x <= 52) {
            lives--;
            // Remove the hexagon from the list
            hexagon_enemies.erase(std::remove_if(hexagon_enemies.begin(), hexagon_enemies.end(), [&it](const HexagonEnemy& hex) {
                return hex.position.x  < 52; 
                }),
                hexagon_enemies.end()
            );
 
            if (lives == 0) {
                printf("Game over!\n");
                printf("Score: %d\n", highScore);
                exit(0);
            }
        }
    }



    if (dragging == true && moving == 1) {
        modelMatrix = glm::mat3(1);
  
        modelMatrix *= transform2D::Translate(clonedDiamondPosition.x, clonedDiamondPosition.y);
        RenderMesh2D(meshes["guiDiamond" + to_string(draggedDiamondIndex)], shaders["VertexColor"], modelMatrix);
    }

    for (int i = 0; i < 4; i++) {
        modelMatrix = glm::mat3(1);
        RenderMesh2D(meshes["guiSquare" + to_string(i)], shaders["VertexColor"], modelMatrix);
        //add stars
        for (int j = 0;j <= i;j++) {
            modelMatrix = glm::mat3(1);
   
            RenderMesh2D(meshes["star" + to_string(i) + to_string(j)], shaders["VertexColor"], modelMatrix);
        }

        modelMatrix = glm::mat3(1);
        RenderMesh2D(meshes["guiDiamond" + to_string(i)], shaders["VertexColor"], modelMatrix);

    }
    //last star
    modelMatrix = glm::mat3(1);
    RenderMesh2D(meshes["star" + to_string(3) + to_string(3)], shaders["VertexColor"], modelMatrix);


    modelMatrix = glm::mat3(1);
    RenderMesh2D(meshes["redRect"], shaders["VertexColor"], modelMatrix);


    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            modelMatrix = glm::mat3(1);
   
            RenderMesh2D(meshes["greenSquare" + to_string(i) + to_string(j)], shaders["VertexColor"], modelMatrix);
        }
    }

    //render lives - red squares
    for (int i = 0;i < lives;i++) {
        modelMatrix = glm::mat3(1);
        RenderMesh2D(meshes["redSquare" + to_string(i)], shaders["VertexColor"], modelMatrix);
    }

    //render stars
    for (int i = 0;i < star_count;i++) {
		modelMatrix = glm::mat3(1);
		RenderMesh2D(meshes["stars_money" + to_string(i)], shaders["VertexColor"], modelMatrix);
	}

    //check if hexagon collided with diamond using glm::distance and if it did, remove diamond
    for (int i = 0;i < hexagon_enemies.size();i++) {
        for (int j = 0;j < 9;j++) {
            for (int k = 0;k < 4;k++) {
                if (readyToPlace[j][k] == 1) {
                    if (glm::distance(hexagon_enemies[i].position, diamondActiveCenters[j * 4 + k]) < 45) {
                     
                        scaledDown[j] = 1;
                      
                    }
                }
            }
        }
    }

    //add active diamond over green square if placed there
    //if right cliked on green square, remove diamond from there
    for (int i = 0;i < 9;i++)
    {
        for (int j = 0;j < 4;j++)
        {
            if (readyToPlace[i][j] == 1 && scaledDown[i] == 0)
            {
                scaleValues[i] = 1;
                modelMatrix = glm::mat3(1);
                RenderMesh2D(meshes["diamond" + to_string(i) + to_string(j)], shaders["VertexColor"], modelMatrix);
            }
            else if (readyToPlace[i][j] == 1 && scaledDown[i] == 1) {
                modelMatrix = glm::mat3(1);
                if (scaleValues[i] > 0) {
                    scaleValues[i] -= deltaTimeSeconds * 2;
                    if (scaleValues[i] < 0) {
                        scaleValues[i] = 0;
                        readyToPlace[i][j] = 0;
                        scaledDown[i] = 0;
                    }
                    //keep position when scaling
                    modelMatrix *= transform2D::Translate(diamondActiveCenters[i * 4 + j].x, diamondActiveCenters[i * 4 + j].y);
                    modelMatrix *= transform2D::Scale(scaleValues[i], scaleValues[i]);
                    modelMatrix *= transform2D::Translate(-diamondActiveCenters[i * 4 + j].x, -diamondActiveCenters[i * 4 + j].y);
                    RenderMesh2D(meshes["diamond" + to_string(i) + to_string(j)], shaders["VertexColor"], modelMatrix);
                }
            }
        }
    }



    for (int i = 0; i < 9; i++) {
        for (int j = 0; j < 4; j++) {
            readyToShoot[i][j] = 0; 
        }
    }


    //check which rows have hexagon of certain colors coming
    for (int i = 0;i < hexagon_enemies.size();i++) {
        if (hexagon_enemies[i].position.y == 110) {
            if (hexagon_enemies[i].color == glm::vec3(0.7, 0, 1))
            {
                readyToShoot[0][0] = 1;
                readyToShoot[1][0] = 1;
                readyToShoot[2][0] = 1;
            }
            if (hexagon_enemies[i].color == glm::vec3(0.8, 0.3, 0.2))
            {
				readyToShoot[0][1] = 1;
				readyToShoot[1][1] = 1;
				readyToShoot[2][1] = 1;
			}
            if (hexagon_enemies[i].color == glm::vec3(0, 0, 1))
            {
                readyToShoot[0][2] = 1;
                readyToShoot[1][2] = 1;
                readyToShoot[2][2] = 1;
            }
            if (hexagon_enemies[i].color == glm::vec3(1, 0.5, 0))
            {
				readyToShoot[0][3] = 1;
				readyToShoot[1][3] = 1;
				readyToShoot[2][3] = 1;
			}
        }
        if (hexagon_enemies[i].position.y == 260) {
            if (hexagon_enemies[i].color == glm::vec3(0.7, 0, 1))
            {
				readyToShoot[3][0] = 1;
				readyToShoot[4][0] = 1;
				readyToShoot[5][0] = 1;
			}
            if (hexagon_enemies[i].color == glm::vec3(0.8, 0.3, 0.2))
            {
                readyToShoot[3][1] = 1;
                readyToShoot[4][1] = 1;
                readyToShoot[5][1] = 1;

            }
            if (hexagon_enemies[i].color == glm::vec3(0, 0, 1))
            {
                readyToShoot[3][2] = 1;
                readyToShoot[4][2] = 1;
                readyToShoot[5][2] = 1;
            }
            if (hexagon_enemies[i].color == glm::vec3(1, 0.5, 0))
            {
				readyToShoot[3][3] = 1;
				readyToShoot[4][3] = 1;
				readyToShoot[5][3] = 1;
			}   
        }
        if (hexagon_enemies[i].position.y == 410) {
            if (hexagon_enemies[i].color == glm::vec3(0.7, 0, 1))
            {
                readyToShoot[6][0] = 1;
                readyToShoot[7][0] = 1;
                readyToShoot[8][0] = 1;
            }
            if (hexagon_enemies[i].color == glm::vec3(0.8, 0.3, 0.2))
            {
				readyToShoot[6][1] = 1;
				readyToShoot[7][1] = 1;
				readyToShoot[8][1] = 1;
			}
            if (hexagon_enemies[i].color == glm::vec3(0, 0, 1))
            {
				readyToShoot[6][2] = 1;
				readyToShoot[7][2] = 1;
				readyToShoot[8][2] = 1;
			}
            if (hexagon_enemies[i].color == glm::vec3(1, 0.5, 0))
            {
                readyToShoot[6][3] = 1;
                readyToShoot[7][3] = 1;
                readyToShoot[8][3] = 1;
            }
        }
    }


    for (int i = 0;i < 9;i++)
    {
        starSpawnTimers[i] += deltaTimeSeconds;
        for (int j = 0;j < 4;j++)
        {
            //if hexagon is on that row and of that color, shoot stars
            if (readyToPlace[i][j] ==1 && readyToShoot[i][j]==1)
            {
                //create star

                starSpawnTimer += deltaTimeSeconds;
                if (starSpawnTimers[i] >= starSpawnRate) {
                    // Reset the timer
                    starSpawnTimers[i] = 0.0f;

                    ShootingStar shooting_star;
                    //i square position
                    shooting_star.position = glm::vec3(leftCornersGreenSquares[i].x + 60, leftCornersGreenSquares[i].y + 60, 14);
                    shooting_star.position_copy = shooting_star.position;
                    float r, g, b;
                    if (j == 0) {
						r = 0.7;
						g = 0;
						b = 1;
					}
                    else if (j == 1) {
						r = 0.8;
						g = 0.3;
						b = 0.2;
					}
                    else if (j == 2) {
						r = 0;
						g = 0;
						b = 1;
					}
                    else {
						r = 1;
						g = 0.5;
						b = 0;
					}
                    shooting_star.color = glm::vec3(r,g,b);
                    shooting_star.speed = 110;
                    shooting_star.star_angle = 0;
                    shooting_star.mesh = object2D::CreateStar("star" + std::to_string(shootingStars.size()), shooting_star.position, 24, 13, shooting_star.color, true);
                    shootingStars.push_back(shooting_star);
                }
            }
        }
    }

    for (auto& star : shootingStars) {
        star.position.x += star.speed * deltaTimeSeconds;
    }

    //render shooting star and make it rotate on its way
    for (auto& sstar : shootingStars) {
        modelMatrix = glm::mat3(1);
        sstar.star_angle += deltaTimeSeconds * 8;
        modelMatrix *= transform2D::Translate(-sstar.position_copy.x+50, 0);
        modelMatrix *= transform2D::Translate(sstar.position.x, 0);

        //rotate around its center while it moves
        modelMatrix *= transform2D::Translate(sstar.position_copy.x, sstar.position_copy.y);
        modelMatrix *= transform2D::Rotate(-sstar.star_angle);
        modelMatrix *= transform2D::Translate(-sstar.position_copy.x, -sstar.position_copy.y);

        RenderMesh2D(sstar.mesh, shaders["VertexColor"], modelMatrix);
    }
    
    //check if shooting star collided with diamond using glm::distance and if it did, remove diamond and star
    for (auto itStar = shootingStars.begin(); itStar != shootingStars.end();) {
        bool collision = false;

        // Loop through hexagon enemies
        for (auto itHex = hexagon_enemies.begin(); itHex != hexagon_enemies.end() && collision==false;) {
            // Check for collision and color match
            if (glm::distance(itStar->position, itHex->position) < 60 && itStar->color == itHex->color) {
                // Collision detected and colors match
                itHex->hits += 1;
                collision = true;
                // Remove the star
                itStar = shootingStars.erase(itStar);

                // Check if the hexagon has been hit 3 times
                if (itHex->hits >= 3) {
                    // Remove the hexagon
                    //scale down hexagon
                    itHex->isScalingDown = true;
                    itHex->scaleSpeed = 3.0f;
          
                }
                else {
                    ++itHex; 
                }
            }
            else {
                ++itHex; 
            }
        }

        if (!collision) {
            ++itStar; 
        }
    }

}


void PvZgame::FrameEnd()
{
}


void PvZgame::OnInputUpdate(float deltaTime, int mods)
{

}


void PvZgame::OnKeyPress(int key, int mods)
{
}


void PvZgame::OnKeyRelease(int key, int mods)
{
}


void PvZgame::OnMouseMove(int mouseX, int mouseY, int deltaX, int deltaY)
{
    if (dragging) {
        moving = 1;
        float y = window->GetResolution().y;
        clonedDiamondPosition.x = mouseX-diamondCenters[draggedDiamondIndex].x;
  
        clonedDiamondPosition.y = -mouseY+ 720-diamondCenters[draggedDiamondIndex].y;
    
    }
}


void PvZgame::OnMouseBtnPress(int mouseX, int mouseY, int button, int mods)
{
        float y = window->GetResolution().y;
        float x = window->GetResolution().x;
    if (button==1) {
        
            if (mouseX >= diamondCenters[0].x - 35 && mouseX <= diamondCenters[0].x + 55
                && mouseY >= 720 - diamondCenters[0].y - 50 && mouseY <= 720 -diamondCenters[0].y + 50) {
                draggedDiamondIndex = 0; // Remember which diamond is being dragged
                clonedDiamondPosition.x = mouseX-diamondCenters[0].x;
                clonedDiamondPosition.y =mouseY+90; // Set the position where the "cloned" diamond will be drawn
                 dragging = true;

            }
            else if (mouseX >= diamondCenters[1].x - 35 && mouseX <= diamondCenters[1].x + 55
                && mouseY >= 720 - diamondCenters[1].y - 50 && mouseY <= 720 - diamondCenters[1].y + 50) {
                draggedDiamondIndex = 1; 
                clonedDiamondPosition.x = mouseX-diamondCenters[1].x;
                clonedDiamondPosition.y = mouseY+90; 
                 dragging = true;
                }
            else if (mouseX >= diamondCenters[2].x - 35 && mouseX <= diamondCenters[2].x + 55
                && mouseY >= 720 - diamondCenters[2].y - 50 && mouseY <= 720 - diamondCenters[2].y + 50) {
                draggedDiamondIndex = 2; 
                clonedDiamondPosition.x = mouseX-diamondCenters[2].x;
                clonedDiamondPosition.y = mouseY+90;
                dragging = true;
                }
            else if (mouseX >= diamondCenters[3].x - 35 && mouseX <= diamondCenters[3].x + 55
                && mouseY >= 720 - diamondCenters[3].y - 50 && mouseY <= 720 - diamondCenters[3].y + 50) {
                draggedDiamondIndex = 3; 
                dragging = true;
               clonedDiamondPosition.x = mouseX-diamondCenters[3].x;
                clonedDiamondPosition.y = mouseY+90;
                
				}
            else
            {


                //check if clicked on stars
                for (int i = 0; i <starCenters.size();i++) {
                    if (mouseX >= starCenters[i].x - 20 && mouseX <= starCenters[i].x + 20
                        && mouseY >= 720 - starCenters[i].y - 20 && mouseY <= 720 - starCenters[i].y + 20)
                    {
                        stars.erase(stars.begin() + i);
                        starCenters.erase(starCenters.begin() + i);
                        star_count++;
                        break;
                    }
                }
            }
 
        }

    if (button == 2) {
       
        if (mouseX >= leftCornersGreenSquares[0].x && mouseX <= leftCornersGreenSquares[0].x+120
            && 720-mouseY >= leftCornersGreenSquares[0].y && 720-mouseY <= leftCornersGreenSquares[0].y+120)
        {
            if (readyToPlace[0][0] == 1 || readyToPlace[0][1] == 1 || readyToPlace[0][2] == 1 ||  readyToPlace[0][3] == 1)
            {
                for (int i = 0;i < 4;i++) {
                    scaledDown[0] = 1;
                }
            }
        }
        if (mouseX >= leftCornersGreenSquares[1].x && mouseX <= leftCornersGreenSquares[1].x + 120
            && 720 - mouseY >= leftCornersGreenSquares[1].y && 720 - mouseY <= leftCornersGreenSquares[1].y + 120)
        {
            if (readyToPlace[1][0] == 1 || readyToPlace[1][1] == 1 || readyToPlace[1][2] == 1 || readyToPlace[1][3] == 1)
            {
                for (int i = 0;i < 4;i++) {
                    scaledDown[1] = 1;
				}
			}
		}
        if (mouseX >= leftCornersGreenSquares[2].x && mouseX <= leftCornersGreenSquares[2].x + 120
            && 720 - mouseY >= leftCornersGreenSquares[2].y && 720 - mouseY <= leftCornersGreenSquares[2].y + 120)
        {
            if (readyToPlace[2][0] == 1 || readyToPlace[2][1] == 1 || readyToPlace[2][2] == 1 || readyToPlace[2][3] == 1)
            {
                for (int i = 0;i < 4;i++) {
                    scaledDown[2] = 1;
                }
            }
        }
        if (mouseX >= leftCornersGreenSquares[3].x && mouseX <= leftCornersGreenSquares[3].x + 120
            && 720 - mouseY >= leftCornersGreenSquares[3].y && 720 - mouseY <= leftCornersGreenSquares[3].y + 120)
        {
            if (readyToPlace[3][0] == 1 || readyToPlace[3][1] == 1 || readyToPlace[3][2] == 1 || readyToPlace[3][3] == 1)
            {
                for (int i = 0;i < 4;i++) {
                    scaledDown[3] = 1;
				}
			}
		}
        if (mouseX >= leftCornersGreenSquares[4].x && mouseX <= leftCornersGreenSquares[4].x + 120
            && 720 - mouseY >= leftCornersGreenSquares[4].y && 720 - mouseY <= leftCornersGreenSquares[4].y + 120)
        {
            if (readyToPlace[4][0] == 1 || readyToPlace[4][1] == 1 || readyToPlace[4][2] == 1 || readyToPlace[4][3] == 1)
            {
                for (int i = 0;i < 4;i++) {
                    scaledDown[4] = 1;
				}
            }
        }
        if (mouseX >= leftCornersGreenSquares[5].x && mouseX <= leftCornersGreenSquares[5].x + 120
            && 720 - mouseY >= leftCornersGreenSquares[5].y && 720 - mouseY <= leftCornersGreenSquares[5].y + 120)
        {
            if (readyToPlace[5][0] == 1 || readyToPlace[5][1] == 1 || readyToPlace[5][2] == 1 || readyToPlace[5][3] == 1)
            {
				for (int i = 0;i < 4;i++) {
                    scaledDown[5] = 1;
                }
            }
        }
        if (mouseX >= leftCornersGreenSquares[6].x && mouseX <= leftCornersGreenSquares[6].x + 120
            && 720 - mouseY >= leftCornersGreenSquares[6].y && 720 - mouseY <= leftCornersGreenSquares[6].y + 120)
        {
            if (readyToPlace[6][0] == 1 || readyToPlace[6][1] == 1 || readyToPlace[6][2] == 1 || readyToPlace[6][3] == 1)
            {
				for (int i = 0;i < 4;i++) {
                    scaledDown[6] = 1;
                }
            }
        }
        if (mouseX >= leftCornersGreenSquares[7].x && mouseX <= leftCornersGreenSquares[7].x + 120
            && 720 - mouseY >= leftCornersGreenSquares[7].y && 720 - mouseY <= leftCornersGreenSquares[7].y + 120)
        {
            if (readyToPlace[7][0] == 1 || readyToPlace[7][1] == 1 || readyToPlace[7][2] == 1 || readyToPlace[7][3] == 1)
            {
                for (int i = 0;i < 4;i++) {
                    scaledDown[7] = 1;
				}
            }
        }
        if (mouseX >= leftCornersGreenSquares[8].x && mouseX <= leftCornersGreenSquares[8].x + 120
            && 720 - mouseY >= leftCornersGreenSquares[8].y && 720 - mouseY <= leftCornersGreenSquares[8].y + 120)
        {
            if (readyToPlace[8][0] == 1 || readyToPlace[8][1] == 1 || readyToPlace[8][2] == 1 || readyToPlace[8][3] == 1)
            {
                for (int i = 0;i < 4;i++) {
                    scaledDown[8] = 1;
                }
            }
        }
    }
   }



void PvZgame::OnMouseBtnRelease(int mouseX, int mouseY, int button, int mods)
{
    if (button == 1 && dragging) {
        dragging = false;
        //check if clonedDiamondPosition is over a green square
        for (int i = 0;i < 9;i++)
        {
            clonedDiamondPosition.x = mouseX;
            clonedDiamondPosition.y =720- mouseY;
            if (clonedDiamondPosition.x > leftCornersGreenSquares[i].x && clonedDiamondPosition.x < leftCornersGreenSquares[i].x + 120
                && clonedDiamondPosition.y > leftCornersGreenSquares[i].y && clonedDiamondPosition.y < leftCornersGreenSquares[i].y + 120 && star_count>= diamond_cost[draggedDiamondIndex])
            {
                if (readyToPlace[i][0] !=1 && readyToPlace[i][1] != 1 && readyToPlace[i][2] != 1 && readyToPlace[i][3] != 1)
                {
					readyToPlace[i][draggedDiamondIndex] = 1;
					//square to place on
					dOverSquareID = i;
                    if (draggedDiamondIndex == 0) {
                        star_count=star_count-1;
                    }
                    else if (draggedDiamondIndex == 1) {
						star_count = star_count - 2;
					}
                    else if (draggedDiamondIndex == 2) {
						star_count = star_count - 2;
					}
                    else {
						star_count = star_count - 3;
					}
                    scaledDown[i] = 0;
					break;
				}
                else
                {
					break;
				}   
            }
        }

    }
}


void PvZgame::OnMouseScroll(int mouseX, int mouseY, int offsetX, int offsetY)
{
}


void PvZgame::OnWindowResize(int width, int height)
{
}

