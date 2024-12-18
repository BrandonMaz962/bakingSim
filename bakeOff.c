#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// Declared shared resources
sem_t pantry;
sem_t refrigerators;
sem_t mixer, bowl, spoon;
sem_t oven;

char *cookie_ingredients[] = {"Flour", "Sugar", "Milk", "Butter"};
char *pancake_ingredients[] = {"Flour", "Sugar", "Baking soda", "Salt", "Egg", "Milk", "Butter"};
char *pizza_ingredients[] = {"Yeast", "Sugar", "Salt"};
char *pretzel_ingredients[] = {"Flour", "Sugar", "Salt", "Yeast", "Baking soda", "Egg"};
char *cinnamon_rolls_ingredients[] = {"Flour", "Sugar", "Salt", "Butter", "Egg", "Cinnamon"};

int color;

// Declare functions
void getIngredient(char *ingredient, int baker);
void makeRecipe(char *ingredientList[], int ingredientNum, int baker, char *recipe);
void bakeRecipes(int baker);

int main()
{
  int numBakers;

  // Get number of bakers from user
  printf("Enter the number of bakers in the kitchen: ");
  scanf("%d", &numBakers);

  // Declare varaibles and semaphores
  pthread_t bakers[numBakers];

  sem_init(&pantry, 0, 1);
  sem_init(&refrigerators, 0, 2);
  sem_init(&mixer, 0, 2);
  sem_init(&oven, 0, 1);
  sem_init(&bowl, 0, 3);
  sem_init(&spoon, 0, 5);

  // Create a thread for each baker
  for (int i = 0; i < numBakers; i++)
  {
    pthread_create(&bakers[i], NULL, bakeRecipes, i);
  }

  // Once the bakers are finished rejoin all of the threads
  for (int i = 0; i < numBakers; i++)
  {
    pthread_join(bakers[i], NULL);
  }

  // Remove all semaphores from shared memory
  sem_destroy(&pantry);
  sem_destroy(&refrigerators);
  sem_destroy(&mixer);
  sem_destroy(&oven);
  sem_destroy(&bowl);
  sem_destroy(&spoon);

  printf("\033[0m");
  printf("All baking has been completed!\n");

  return 0;
}

void getIngredient(char *ingredientName, int bakerId)
{
  color = 31 + (bakerId % 8);
  printf("\033[0;%dm", color);
  printf("Baker #%d is getting the ingredient: %s\n", bakerId, ingredientName);

  if (ingredientName == "Flour" || ingredientName == "Sugar" || ingredientName == "Baking soda" || ingredientName == "Salt" || ingredientName == "Yeast" || ingredientName == "Cinnamon")
  {
    sem_wait(&pantry);

    printf("\033[0;%dm", color);
    printf("Baker #%d is getting %s from the pantry\n", bakerId, ingredientName);

    usleep(1000000); // Need to set as time to fetch ingredient
    sem_post(&pantry);
  }
  else
  {
    sem_wait(&refrigerators);

    printf("\033[0;%dm", color);
    printf("Baker #%d is getting %s from the refrigerator\n", bakerId, ingredientName);

    usleep(1000000); // Need to set as time to fetch ingredient
    sem_post(&refrigerators);
  }
}

void makeRecipe(char *ingredientList[], int ingredientNum, int bakerId, char *recipeName)
{
  color = 31 + (bakerId % 8);
  printf("\033[0;%dm", color);
  printf("Baker #%d is starting to make: %s\n", bakerId, recipeName);

  // Collect Ingredients
  for (int i = 0; i < ingredientNum; i++)
  {
    getIngredient(ingredientList[i], bakerId);
  }

  // Once ingredients have been aquired collect the tools
  sem_wait(&bowl);
  color = 31 + (bakerId % 8);
  printf("\033[0;%dm", color);
  printf("Baker #%d acquired a bowl for making: %s\n", bakerId, recipeName);

  sem_wait(&spoon);
  color = 31 + (bakerId % 8);
  printf("\033[0;%dm", color);
  printf("Baker #%d acquired a spoon for making: %s\n", bakerId, recipeName);

  sem_wait(&mixer);
  color = 31 + (bakerId % 8);
  printf("\033[0;%dm", color);
  printf("Baker #%d acquired a mixer for making: %s\n", bakerId, recipeName);

  // Once ingredients and tools have been aquired put into oven
  sem_wait(&oven);
  color = 31 + (bakerId % 8);
  printf("\033[0;%dm", color);
  printf("Baker #%d has started baking %s in the oven\n", bakerId, recipeName);

  usleep(3000000); // Need to set time for baking

  sem_post(&oven);
  color = 31 + (bakerId % 8);
  printf("\033[0;%dm", color);
  printf("Baker #%d has finished baking: %s\n", bakerId, recipeName);

  // Once finished free up used tools
  color = 31 + (bakerId % 8);
  printf("\033[0;%dm", color);
  printf("Baker #%d has finished using bowl for: %s\n", bakerId, recipeName);
  printf("Baker #%d has finished using spoon for: %s\n", bakerId, recipeName);
  printf("Baker #%d has finished using mixer for: %s\n", bakerId, recipeName);

  sem_post(&bowl);
  sem_post(&spoon);
  sem_post(&mixer);
}

void bakeRecipes(int bakerId)
{
  color = 31 + (bakerId % 8);
  printf("\033[0;%dm", color);
  printf("Baker #%d is starting to bake!\n", bakerId);

  // Make each recipe
  makeRecipe(cookie_ingredients, 4, bakerId, "Cookies");
  makeRecipe(pancake_ingredients, 7, bakerId, "Pancakes");
  makeRecipe(pizza_ingredients, 3, bakerId, "Pizza Dough");
  makeRecipe(pretzel_ingredients, 6, bakerId, "Soft Pretzels");
  makeRecipe(cinnamon_rolls_ingredients, 6, bakerId, "Cinnamon Rolls");

  color = 31 + (bakerId % 8);
  printf("\033[0;%dm", color);
  printf("Baker #%d has finished baking all recipies!\n", bakerId);
}