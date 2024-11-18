import time
import threading
from dotenv import load_dotenv
import os
import google.generativeai as genai
import re


load_dotenv()

gemini_api_key = os.getenv("GEMINI_API_KEY")

genai.configure(api_key=gemini_api_key)

def process_response(response_text):
    quiz_data = []
    questions = response_text.strip().split('\n\n')
    
    for question_block in questions:
        lines = question_block.strip().split('\n')
        question_text = lines[0].strip()
        options = []
        answer = None
        
        for line in lines[1:]:
            if re.match(r'^\d+\.', line.strip()):
                options.append(line.strip())
            elif line.strip().startswith('Answer:'):
                answer = line.strip().split('Answer:')[1].strip()
            elif line.strip().startswith('answer:'):
                answer = line.strip().split('answer:')[1].strip()
            elif line.strip().startswith('Correct answer:'):
                answer = line.strip().split('Correct answer:')[1].strip()
            elif line.strip().startswith('Correct:'):
                answer = line.strip().split('Correct:')[1].strip()
            elif line.strip().startswith('correct:'):
                answer = line.strip().split('correct:')[1].strip()
            elif line.strip().startswith('Correct Answer:'):
                answer = line.strip().split('Correct Answer:')[1].strip()
            elif line.strip().startswith('correct answer:'):
                answer = line.strip().split('correct answer:')[1].strip()
            elif line.strip().startswith('correct:'):
                answer = line.strip().split('correct:')[1].strip()
            elif line.strip().startswith('correct Answer:'):
                answer = line.strip().split('correct Answer:')[1].strip()
            elif line.startswith('Correct Option:'):
                answer = line.split('Correct Option:')[1].strip()
            elif line.startswith('Correct option:'):
                answer = line.split('Correct option:')[1].strip()
            elif line.startswith('correct option:'):
                answer = line.split('correct option:')[1].strip()
            elif line.strip().startswith('**'):
                answer = line.strip().split('**')[1].strip()
            
            
        
        quiz_data.append({
            'question': question_text,
            'options': options,
            'answer': answer
        })
    
    return quiz_data

def remove_numbering(text):
    return re.sub(r'^\d+\.\s*', '', text)

def generate_quiz(subject, no_of_questions):
    print("Generating quiz for subject: " + subject + " with " + no_of_questions + " questions")

    generation_config = {
        "temperature": 0.9,
        "top_p": 1,
        "top_k": 1,
        "max_output_tokens": 2048,
    }

    safety_settings = [
        {"category": "HARM_CATEGORY_HARASSMENT", "threshold": "BLOCK_ONLY_HIGH"},
        {"category": "HARM_CATEGORY_HATE_SPEECH", "threshold": "BLOCK_ONLY_HIGH"},
        {"category": "HARM_CATEGORY_SEXUALLY_EXPLICIT", "threshold": "BLOCK_ONLY_HIGH"},
        {"category": "HARM_CATEGORY_DANGEROUS_CONTENT", "threshold": "BLOCK_ONLY_HIGH"},
    ]

    model = genai.GenerativeModel(
        model_name="gemini-pro",
        generation_config=generation_config,
        safety_settings=safety_settings,
    )

    print(subject, no_of_questions)

    response = model.generate_content(f"Generate a quiz for {subject} with {no_of_questions} questions. Dont use formatting while responding. use numbers for numbering options. dont use asterisk. in format of question, next 4 lines are options and next line is just the correct option number")
    
    response = response.text

    print(response)

    quiz_data = process_response(response)

    for question in quiz_data:
        print("Question:", question['question'])
        for option in question['options']:
            print("Option:", option)
        print("Answer:", question['answer'])
        print()
    
    # write to file
    with open('quiz_with_ai_output.txt', 'w') as file:
        for question in quiz_data:
            # Remove numbering from question
            question_text = remove_numbering(question['question'])
            file.write(question_text + "\n")
            for option in question['options']:
                # Remove numbering from options
                option_text = remove_numbering(option)
                file.write(option_text + "\n")
            file.write(question['answer'] + "\n")

    print("Quiz generated successfully")




def AI_quiz_maker_loop():
    while(1):
        with open('quiz_with_ai_input.txt', 'r') as file:
            query = file.read().strip()

        if query != "":
            # search_single_song(query)
            
            print(query)
            subject = query.split("\n")[0].strip()
            print(subject)
            no_of_questions = query.split("\n")[1].strip()
            print(no_of_questions)

            # AI logic to generate quiz
            generate_quiz(subject, no_of_questions)

            with open('quiz_with_ai_input.txt', 'w') as file:
                file.write("")

        time.sleep(1)

thread1 = threading.Thread(target=AI_quiz_maker_loop)
thread1.start()

# generate_quiz("queues data structure in c", "4")