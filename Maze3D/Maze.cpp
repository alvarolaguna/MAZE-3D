#include "Maze.h"

using namespace std;

Maze::Maze(int theme)
{

	Maze::theme = theme;
}

void Maze::build()
{
}

bool Maze::checkMove(int x, int y)
{
	bool temp = grid[x][y] == 0;
	cout << temp  << endl;
	return grid[x][y] == 0;
}

int Maze::getGridHeight()
{
	return sizeof(grid)/sizeof(*grid);
	//(sizeof(p)/sizeof(*p))
}

int Maze::getGridWidth()
{
	return sizeof(grid[0])/ sizeof(*grid[0]);
}

int Maze::getCoordValue(int y, int x)
{
	return grid[y][x];
}

void Maze::setCoordValue(int y, int x, int value)
{
	grid[y][x] = value;
}

void Maze::setLight(int material)
{
	//Definiendo propiedades de luces
	glEnable(GL_DEPTH_TEST);
	//Fuente de luz 0
	float light_ambient[] = { 0.,0.,0.,1. };
	float light_diffuse[] = { 1.,1.,1.,1. };
	float light_specular[] = { 1.,1.,1.,1. };
	float light_position[] = { 1.,1.,1.,1. };

	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	setMaterial(material);

	//Definir el modelo de iluminación y sombreado
	float global_light_ambient[] = { .2,.2,.2,1. };
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, global_light_ambient);

	float local_viewer[] = { 0. }; //0 infinito, 1 local
	glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_viewer);

	float two_side[] = { 0. };	//0 two side ilum = false
	glLightModelfv(GL_LIGHT_MODEL_TWO_SIDE, two_side);

	glShadeModel(GL_SMOOTH); //Sombreado por interpolado de Gouroud

								//Habilitar la iluminación en OpenGL y las fuentes de luz
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

}

void Maze::setMaterial(int material)
{
	float material_ambient[4];
	float material_diffuse[4];
	float material_specular[4];
	float material_shininess[1];
	switch (material)
	{
	case jade:
		
		material_ambient[0] = .135;
		material_ambient[1] = .2225;
		material_ambient[2] = .1575;
		material_ambient[3] = .1;

		material_diffuse[0] = .54;
		material_diffuse[1] = .89;
		material_diffuse[2] = .63;
		material_diffuse[3] = .1;

		material_specular[0] = .316228;
		material_specular[1] = .316228;
		material_specular[2] = .316228;
		material_specular[3] = .1;

		material_shininess[0] = 12.8;
		break;
	case chrome:
		//Material Chrome
		material_ambient[0] = .25;
		material_ambient[1] = .25;
		material_ambient[2] = .25;
		material_ambient[3] = .1;

		material_diffuse[0] = .4;
		material_diffuse[1] = .4;
		material_diffuse[2] = .4;
		material_diffuse[3] = .1;

		material_specular[0] = .774597;
		material_specular[1] = .774597;
		material_specular[2] = .774597;
		material_specular[3] = .1;

		material_shininess[0] = 76.8;
		break;
	case ruby:
		//Material Chrome
		material_ambient[0] = .1745;
		material_ambient[1] = .01175;
		material_ambient[2] = .01175;
		material_ambient[3] = .1;

		material_diffuse[0] = .61424;
		material_diffuse[1] = .04136;
		material_diffuse[2] = .04136;
		material_diffuse[3] = .1;

		material_specular[0] = .727811;
		material_specular[1] = .626959;
		material_specular[2] = .626959;
		material_specular[3] = .1;

		material_shininess[0] = 76.8;
		break;
	case turquoise:
		//Material Chrome
		material_ambient[0] = .1;
		material_ambient[1] = .18725;
		material_ambient[2] = .1745;
		material_ambient[3] = .1;

		material_diffuse[0] = .396;
		material_diffuse[1] = .74151;
		material_diffuse[2] = .69102;
		material_diffuse[3] = .1;

		material_specular[0] = .297254;
		material_specular[1] = .30829;
		material_specular[2] = .306678;
		material_specular[3] = .1;

		material_shininess[0] = 12.8;
		break;
		
	}
	

	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, material_ambient);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, material_diffuse);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, material_specular);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SHININESS, material_shininess);
}

void Maze::setLevel(int level)
{
	switch (level) 
	{
	case easy:
		for (int i = 0; i < getGridHeight(); i++)
		{
			for (int j = 0; j < getGridWidth(); j++)
			{
				setCoordValue(i, j, gridEasy[i][j]);
			}
		}
		break;
	case medium:
		for (int i = 0; i < getGridHeight(); i++)
		{
			for (int j = 0; j < getGridWidth(); j++)
			{
				setCoordValue(i, j, gridMedium[i][j]);
			}
		}
		break;
	case hard:
		for (int i = 0; i < getGridHeight(); i++)
		{
			for (int j = 0; j < getGridWidth(); j++)
			{
				setCoordValue(i, j, gridHard[i][j]);
			}
		}
		break;
	}

}




